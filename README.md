# fons-instrumentorum
Using LLVM to achieve some basic source code manipulation. Trying to detail the steps so that noobs like me can also use the tool (whose documentation noobs might not necessarily like)

## Quick-note
As indicated by the `makefile`, the directory of this repo should be at the same level as the `llvm-project`. The latter is the official release of `llvm`. Resolve the path if necessary. 

## Run demo
After `make`, run `./example test_bare_loop.c`. This would print the information regarding the `main` in the source file, including basicblock info, loop entry point, and associated line number, to the `stderr`. 
### Analyze other functions
`./example test_func_loop.c -funcname add`
### Code instrumentation at the loop entry
`./example test_func_loop.c -funcname main -instrument 'CHECK()'
This would print the transformed code to `stdout`. Thus run `./example test_func_loop.c -funcname main -instrument 'CHECK()' > transformed.c` to save the resultant code. 
