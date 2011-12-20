#CXX := clang++
LLVMCOMPONENTS := backend
RTTIFLAG := -fno-rtti
#RTTIFLAG :=
CXXFLAGS := $(shell llvm-config --cxxflags) $(RTTIFLAG)
LLVMLDFLAGS := $(shell llvm-config --ldflags --libs $(LLVMCOMPONENTS))
DDD := $(shell echo $(LLVMLDFLAGS))
SOURCES = CItutorial1.cpp \
    CItutorial2.cpp \
    CItutorial3.cpp \
    CItutorial4.cpp \
    CItutorial6.cpp

OBJECTS = $(SOURCES:.cpp=.o)
EXES = $(OBJECTS:.o=)
CLANGLIBS = \
    -lclangFrontend \
    -lclangParse \
    -lclangSema \
    -lclangAnalysis \
    -lclangAST \
    -lclangLex \
    -lclangBasic \
    -lclangDriver \
    -lclangSerialization \


all: $(OBJECTS) $(EXES)

%: %.o
	$(CXX) -o $@ $<   $(CLANGLIBS) $(LLVMLDFLAGS)
	#$(CXX) -o $@ $<  -lLLVM-3.0 -lclang

clean:
	-rm -f $(EXES) $(OBJECTS) *~
