

#ifndef LLVM_TRANSFORMS_PRINTF_PASS_H
#define LLVM_TRANSFORMS_PRINTF_PASS_H

#include "llvm/IR/PassManager.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"

namespace llvm {

class PrintfPass : public PassInfoMixin<PrintfPass> {
public:
  PreservedAnalyses run(Module &F, ModuleAnalysisManager &AM);
};

} // namespace llvm

#endif // LLVM_TRANSFORMS_PRINTF_PASS_H

