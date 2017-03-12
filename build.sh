#!/bin/bash

# ../emsdk_portable/emscripten/1.35.0/emcc --help
clear
../../emsdk_portable/emscripten/1.35.0/emmake make clear
../../emsdk_portable/emscripten/1.35.0/emmake make
../../emsdk_portable/emscripten/1.35.0/emcc output/main.bc --preload-file resources -o output/main.html
if [ $? -eq 0 ]; then
    open output/main.html
fi
