# About #
This is a collection of tutorials showing off how to use core Clang types. It is based directly on two older tutorials which no longer built due to code rot.

1. [tutorial 1](http://amnoid.de/tmp/clangtut/tut.html)
2. [tutorial 2](http://www.cs.rpi.edu/~laprej/clang.html)

This particular set of tutorials tracks the llvm / clang mainline and is updated semi-regularly to account for llvm / clang API changes.

See contents of the links above for a walkthrough of what these tutorials are doing.
# CI tutorials #
The tutorials prefixed with CI are the same as the original tutorials but use the CompilerInstance object and
its helper methods to perform the same tasks as the original tutorials. For the most part, this makes the code
much more compact.

# Windows Build #
Note on the Windows build: Currently the paths are hardcoded. Please see the SharedBuild.targets file
inside of the SharedBuild project to update the path for your specific LLVM and CLANG install.

In particular, the LLVMLibsDirs property and the LLVMIncludes property should be updated.

# Contact Me #
For any questions, please ping me via my github account. Changes and additions are always welcome.
