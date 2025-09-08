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
    regionBody.walk([&](Operation *op) {
      LDBG(5) << "Visiting operation: " << *op;
      // 1. if an operation has an attribute named "my.flag", then remove it and
      // add instead a "my.flagged" attribute.
      if (op->getDiscardableAttr("my.flag")) {
        LDBG("my-pass-attr-update", 2)
            << "Removing attribute my.flag from " << *op;
        op->removeDiscardableAttr("my.flag");
        op->setDiscardableAttr("my.flagged", UnitAttr::get(&getContext()));
        LDBG("my-pass-attr-update", 2) << "After updating Op " << *op;
      }

      // 2. find all ops named "my.sub" and swap the operands
      if (op->getName().getStringRef() == "my.sub") {
        LDBG("my-pass-sub-update", 2) << "Swapping operands of " << *op;
        std::swap(op->getOpOperand(0), op->getOpOperand(1));
        LDBG("my-pass-sub-update", 2) << "After swapping operands: " << *op;
      }

      // 3. find all ops named "my.add" and rewrite them into a
      // new operation "my.super_add"
      if (op->getName().getStringRef() == "my.add") {
        LDBG("my-pass-add-update", 2) << "Rewriting " << *op;
        // This is a low-level API, you will never use this one in practice
        // when you define you own dialect.
        OperationState state(op->getLoc(), "my.super_add", op->getOperands(),
                             op->getResultTypes());
        // This is the raw API to create an operation, but it would
        // be orphan. It's like using a naked `new` operator.
        // We prefer RAII, and so using a builder here is playing
        // this role by inserting the operation right next to the
        // one we visit.
        // Operation *newOp = Operation::create(state);
        OpBuilder builder(op);
        Operation *newOp = builder.create(state);
        op->replaceAllUsesWith(newOp);
        LDBG("my-pass-add-update", 2) << "New Operation " << *newOp;
      }

      // 4. Delete all operations that don't have any user
      if (op->getUsers().empty()) {
        LDBG("my-pass-dce", 2) << "Deleting " << *op;
        op->erase();
      }
    });
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
