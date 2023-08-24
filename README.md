# fons-instrumentorum
Using LLVM to achieve some basic source code manipulation. Trying to detail the steps so that noobs like me can also use the tool (whose documentation noobs might not necessarily like)

## Quick-note
As indicated by the `makefile`, the directory of this repo should be at the same level as the `llvm-project`. The latter is the [official release](https://llvm.org/docs/GettingStarted.html) of `llvm`. Resolve the path if necessary. 

## Run demo
After `make`, run `./example test_bare_loop.c`. This would print the information regarding the `main` in the source file, including basicblock info, loop entry point, and associated line number, to the `stderr`. 
### Analyze other functions
`./example test_func_loop.c -funcname add`
### Code instrumentation at the loop entry
`./example test_func_loop.c -funcname main -instrument 'CHECK()'`
This would print the transformed code to `stdout`. Thus run `./example test_func_loop.c -funcname main -instrument 'CHECK()' > transformed.c` to save the resultant code. The resultant code would have `CHECK()` before each loop entry point. 

## Common Error
### `make` success, but execution complain missing `XXX.so.XXX`
Likely the missing library is in the `LLVM_BUILD_PATH/lib`. Thus run `export LD_LIBRARY_PATH=LLVM_BUILD_PATH/lib`
### code runs, but complain no compilation database
If you remove the `compile_commands.json` or analyze other source code, it would complain. I suggest read [this](https://eli.thegreenplace.net/2014/05/21/compilation-databases-for-clang-based-tools/).
Since this repo use `makefile`, by `bear make`, the corresponding compilation database can be generated. 
### llvm build failed
As for myself, after using a newer version of `cmake` solved the problem. 
### undefined reference to xxx.h
`updatedb`
`locate xxx.h`
Find the one from `llvm` build directory and include it. 
### undefined reference to xxxxxxxx()
in the `llvm` build dir, `grep -ir xxxxxxxx()`
Figure out which file should also be included. 
