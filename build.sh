#!/bin/bash

# ../emsdk_portable/emscripten/1.35.0/emcc --help
clear
../../emsdk_portable/emscripten/1.35.0/emmake make clear
../../emsdk_portable/emscripten/1.35.0/emmake make
../../emsdk_portable/emscripten/1.35.0/emcc output/main.bc -O2 -s ALLOW_MEMORY_GROWTH=1 --preload-file resources -o output/main.html
