#!/bin/bash

cp src/PrintfPass.h ../llvm/include/llvm/Transforms/Utils/PrintfPass.h
cp src/PassBuilder.cpp ../llvm/lib/Passes/PassBuilder.cpp
cp src/PassRegistry.def ../llvm/lib/Passes/PassRegistry.def
cp src/CMakeLists.txt ../llvm/lib/Transforms/Utils/CMakeLists.txt
cp src/PrintfPass.cpp ../llvm/lib/Transforms/Utils/PrintfPass.cpp
