#include "llvm/Transforms/Utils/PrintfPass.h"

using namespace llvm;

// module pass because we need to declare functions/global variables
PreservedAnalyses PrintfPass::run(Module &M, ModuleAnalysisManager &AM) {
  // construct a generic integer pointer type
  PointerType *PointerTy =
      PointerType::get(IntegerType::get(M.getContext(), 32), 0);

  // declare fopen as an externally linked function
  // construct type
  auto *fopen_type =
      FunctionType::get(PointerTy, {PointerTy, PointerTy}, false);
  // insert function into module
  auto fopen_value = M.getOrInsertFunction("fopen", fopen_type).getCallee();
  auto fopen_funct = dyn_cast<Function>(fopen_value);

  // declare fprintf as an externally linked function
  auto fprintf_type = FunctionType::get(IntegerType::get(M.getContext(), 32),
                                        {PointerTy, PointerTy}, true);
  auto fprintf_value =
      M.getOrInsertFunction("fprintf", fprintf_type).getCallee();
  auto fprintf_funct = dyn_cast<Function>(fprintf_value);

  // declare fclose as an externally linked function
  auto fclose_type = FunctionType::get(IntegerType::get(M.getContext(), 32),
                                       {PointerTy}, false);
  auto fclose_value = M.getOrInsertFunction("fclose", fclose_type).getCallee();
  auto fclose_funct = dyn_cast<Function>(fclose_value);

  // declare a global variable to keep file descriptor for log.txt
  GlobalVariable *global_file_descriptor =
      new GlobalVariable(/*Module=*/M,
                         /*Type=*/PointerTy,
                         /*isConstant=*/false,
                         /*Linkage=*/GlobalValue::CommonLinkage,
                         /*Initializer=*/0, // has initializer, specified below
                         /*Name=*/"log_file_descriptor");
  ConstantPointerNull *const_ptr_2 = ConstantPointerNull::get(PointerTy);
  global_file_descriptor->setInitializer(const_ptr_2);

  // iterate through all functions in module
  for (auto &F : M) {
    // encounter main function
    if (F.getName() == "main") {
      // call fopen at the beginning of first BB
      auto BB = &F.getEntryBlock();
      IRBuilder<> builder(BB);
      auto first_instr = BB->getFirstNonPHI();
      builder.SetInsertPoint(first_instr);
      llvm::Value *filename = builder.CreateGlobalString("log.txt");
      llvm::Value *openmode = builder.CreateGlobalString("w");
      auto fopen_call_instr =
          builder.CreateCall(fopen_funct, {filename, openmode});
      builder.CreateStore(fopen_call_instr, global_file_descriptor);

      // iterate through basic blocks -> call fclose before each return
      // statement
      for (auto &BB : F) {
        for (auto &I : BB) {
          if (ReturnInst *RI = dyn_cast<ReturnInst>(&I)) {
            auto previous_instr = RI->getPrevNode();
            // IRBuilder<> builder(previous_instr);
            builder.SetInsertPoint(RI);
            builder.CreateCall(fclose_funct, {fopen_call_instr});
          }
        }
      }
    }
    // for every function iterate through instructions and insert fprintf calls
    // whenever printf calls are detected
    for (auto &BB : F) {
      for (auto &Instr : BB) {
        if (auto *CB = dyn_cast<CallBase>(&Instr)) {
          auto calledFunc = CB->getCalledFunction();
          if (calledFunc->getName() == "printf") {
            IRBuilder<> builder(CB);
            builder.SetInsertPoint(CB);
            std::vector<llvm::Value *> arg_vec;
            auto file_desc =
                builder.CreateLoad(PointerTy, global_file_descriptor);
            arg_vec.push_back(file_desc);
            for (auto arg = CB->arg_begin(); arg != CB->arg_end(); ++arg) {
              auto val = dyn_cast<llvm::Value>(arg);
              arg_vec.push_back(val);
            }
            builder.CreateCall(fprintf_funct, arg_vec);
          }
        }
      }
    }
  }
  return PreservedAnalyses::all();
}
