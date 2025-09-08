#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/IR/BuiltinAttributes.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/OwningOpRef.h>
#include <mlir/Parser/Parser.h>
#include <mlir/Pass/Pass.h>
#include <mlir/Tools/mlir-opt/MlirOptMain.h>

#include <llvm/Support/DebugLog.h>
#include <llvm/Support/raw_ostream.h>

#define DEBUG_TYPE "my-pass"

using namespace mlir;

class MyPass : public PassWrapper<MyPass, OperationPass<>> {
public:
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(MyPass)
  // This is how we'll refer to the pass on the command line.
  StringRef getArgument() const override { return "my-pass"; }
  StringRef getDescription() const override {
    return "This pass is printing the operation recursively";
  }

  void runOnOperation() override {
    if (!getOperation()->getNumRegions()) {
      getOperation()->emitWarning()
          << "Current operation has no region: " << *getOperation() << "\n";
      return;
    }
    Region &regionBody = getOperation()->getRegion(0);
    if (regionBody.empty()) {
      getOperation()->emitWarning()
          << "Current operation has an empty region: " << *getOperation()
          << "\n";
      return;
    }

    // TODO: Let's mutate the IR here.
    // 1. if an operation has an attribute named "my.flag", then remove it and
    // add instead a "my.flagged" attribute.
    // 2. find all ops named "my.sub" and swap the operands
    // 3. find all ops named "my.add" and rewrite them into a
    // new operation "my.super_add"
    // 4. Delete all operations that don't have any user
  }
};

int main(int argc, char **argv) {
  // This registration is making the pass accessible from the command line.
  PassRegistration<MyPass>();

  DialectRegistry registry;
  registry.insert<func::FuncDialect>();
  return asMainReturnCode(
      MlirOptMain(argc, argv, "Pass Management Driver\n", registry));

  return 0;
}
