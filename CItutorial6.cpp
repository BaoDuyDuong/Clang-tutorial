/***   tutorial4_CI.cpp   *****************************************************
 * This code is licensed under the New BSD license.
 * See LICENSE.txt for details.
 * 
 * The CI tutorials remake the original tutorials but using the
 * CompilerInstance object which has as one of its purpose to create commonly
 * used Clang types.
 *****************************************************************************/
#include <iostream>

#include "llvm/Support/Host.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Parse/Parser.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Frontend/HeaderSearchOptions.h"

/******************************************************************************
 *
 *****************************************************************************/
class MyASTConsumer : public clang::ASTConsumer
{
public:
    MyASTConsumer() : clang::ASTConsumer() { }
    virtual ~MyASTConsumer() { }

    virtual void HandleTopLevelDecl( clang::DeclGroupRef d)
    {
        static int count = 0;
        clang::DeclGroupRef::iterator it;
        for( it = d.begin(); it != d.end(); it++)
        {
            count++;
            clang::VarDecl *vd = llvm::dyn_cast<clang::VarDecl>(*it);
            if(!vd)
            {
                continue;
            }
            if( vd->isFileVarDecl() && vd->hasExternalStorage() )
            {
                std::cerr << "Read top-level variable decl: '";
                std::cerr << vd->getDeclName().getAsString() ;
                std::cerr << std::endl;
            }
        }
    }
};

class TestConsumer : public clang::ASTConsumer, public clang::RecursiveASTVisitor<TestConsumer>
{
public:
    TestConsumer() : clang::ASTConsumer() {}
    bool VisitCallExpr (clang::CallExpr *E) {
     //llvm::errs() << "Function call maybe?! " << E->getNumArgs() << "\n";
     return true;
   }
   
   virtual void HandleTranslationUnit(clang::ASTContext &ctx) {
     TraverseDecl(ctx.getTranslationUnitDecl());
   }
};

class KDevDiagnosticConsumer : public clang::DiagnosticConsumer
{
public:
        virtual void HandleDiagnostic(clang::DiagnosticsEngine::Level DiagLevel,
                                      const clang::Diagnostic &Info);
        virtual DiagnosticConsumer *clone(clang::DiagnosticsEngine &Diags) const;
};

void KDevDiagnosticConsumer::HandleDiagnostic(clang::DiagnosticsEngine::Level DiagLevel,
                                                      const clang::Diagnostic &Info)
{
        llvm::SmallVectorImpl<char> str(1);
        Info.FormatDiagnostic(str);
        llvm::errs() << "Got diagnostic " << Info.getSourceManager().getSpellingLineNumber(Info.getLocation()) << " " << Info.getSourceManager().getSpellingColumnNumber(Info.getLocation())<< " " << &str[0] << " " << Info.getNumArgs() << " " << Info.getNumRanges() << "\n";
        llvm::errs() << "foo\n";
}

clang::DiagnosticConsumer *KDevDiagnosticConsumer::clone(clang::DiagnosticsEngine &Diags) const
{
        return new KDevDiagnosticConsumer();
}

/******************************************************************************
 *
 *****************************************************************************/
int main()
{
    using clang::CompilerInstance;
    using clang::TargetOptions;
    using clang::TargetInfo;
    using clang::FileEntry;
    using clang::Token;
    using clang::ASTContext;
    using clang::ASTConsumer;
    using clang::Parser;

    CompilerInstance ci;
    ci.createDiagnostics(0,NULL);
    //ci.createDiagnostics(0,NULL, new KDevDiagnosticConsumer());

    ci.getHeaderSearchOpts().UseStandardSystemIncludes = true;
    ci.getHeaderSearchOpts().AddPath("/usr/lib/clang/3.0/include/", clang::frontend::Angled, false, false, false);
    ci.getHeaderSearchOpts().AddPath("/usr/include/", clang::frontend::Angled, false, false, false);
    ci.getHeaderSearchOpts().AddPath("/usr/include/linux", clang::frontend::Angled, false, false, false);
    ci.getHeaderSearchOpts().AddPath("/usr/include/c++/4.6.2", clang::frontend::Angled, false, false, false);
    ci.getHeaderSearchOpts().AddPath("/usr/include/c++/4.6.2/tr1", clang::frontend::Angled, false, false, false);
    ci.getHeaderSearchOpts().AddPath("/usr/include/c++/4.6.2/x86_64-unknown-linux-gnu", clang::frontend::Angled, false, false, false);

    ci.getLangOpts().C99 = 1;
    ci.getLangOpts().C1X = 1;
    ci.getLangOpts().GNUMode = 1;
    ci.getLangOpts().GNUKeywords = 1;
    ci.getLangOpts().CPlusPlus = 1;
    ci.getLangOpts().Bool = 1;
    ci.getLangOpts().NoBuiltin = 0;
    TargetOptions to;
    to.Triple = llvm::sys::getHostTriple();

    TargetInfo *pti = TargetInfo::CreateTargetInfo(ci.getDiagnostics(), to);
    clang::Builtin::Context builtinContext;
    //builtinContext.InitializeBuiltins(identifierTable, ci.getLangOpts());
    builtinContext.InitializeTarget(*pti);
    ci.setTarget(pti);
    

    /*
    const clang::Builtin::Info *records;
    unsigned nRecords;
    ci.getTarget().getTargetBuiltins(records, nRecords);
    for (int i = 0; i < nRecords; i++)
	llvm::outs() << records[i].Name << "\n";
    llvm::outs() << nRecords;
    return 0;
    */


    ci.createFileManager();
    ci.createSourceManager(ci.getFileManager());
    ci.createPreprocessor();
    builtinContext.InitializeBuiltins(ci.getPreprocessor().getIdentifierTable(), ci.getLangOpts());
    ci.getPreprocessorOpts().UsePredefines = true;

    ci.getHeaderSearchOpts().Verbose = 1;
    
    //MyASTConsumer *astConsumer = new MyASTConsumer();
    TestConsumer *astConsumer = new TestConsumer();
    ci.setASTConsumer(astConsumer);

    ci.createASTContext();

	const FileEntry *pFile = ci.getFileManager().getFile("test.c");
    ci.getSourceManager().createMainFileID(pFile);
    ci.getDiagnosticClient().BeginSourceFile(ci.getLangOpts(),
                                             &ci.getPreprocessor());
    clang::ParseAST(ci.getPreprocessor(), astConsumer, ci.getASTContext());
    ci.getDiagnosticClient().EndSourceFile();

    return 0;
}
