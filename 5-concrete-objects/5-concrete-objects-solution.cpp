#include <mlir/Dialect/Arith/IR/Arith.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/IR/BuiltinAttributes.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/OwningOpRef.h>
#include <mlir/IR/PatternMatch.h>
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

    // Let's mutate the IR here.
    getOperation()->walk([&](Operation *operation) {
      // 1. if an operation is FuncOp, print the name of the function.
      if (func::FuncOp funcOp = dyn_cast<func::FuncOp>(operation)) {
        llvm::errs() << "Function name: " << funcOp.getName() << "\n";
        return;
      }
      // 2. if an operation is arith.fadd add a FastMathFlags
      // 'reassoc' attribute.
      if (arith::AddFOp addOp = dyn_cast<arith::AddFOp>(operation)) {
        addOp.setFastmath(arith::FastMathFlags::reassoc);
        return;
      }
      // 3. find all arith::SubOp and swap the operands
      if (arith::SubFOp subOp = dyn_cast<arith::SubFOp>(operation)) {
        Value lhs = subOp.getLhs();
        Value rhs = subOp.getRhs();
        subOp.getLhsMutable().set(rhs);
        subOp.getRhsMutable().set(lhs);
        // or:
        subOp->setOperand(0, rhs);
        subOp->setOperand(1, lhs);
        // or: (!!! you need `getLhsMutable()` instead of `getLhs()`)
        if (false)
          std::swap(subOp.getLhsMutable(), subOp.getRhsMutable());
        return;
      }
      // 4. find all arith::DivFOp and rewrite them into a
      //    new ArithMulOp operation
      if (arith::DivFOp divOp = dyn_cast<arith::DivFOp>(operation)) {
#if 1
        IRRewriter rewriter(divOp);
        rewriter.replaceOpWithNewOp<arith::MulFOp>(divOp, divOp.getLhs(),
                                                   divOp.getRhs());
        // Or:
#else
        OpBuilder builder(divOp);
        arith::MulFOp mulOp = arith::MulFOp::create(
              builder, divOp.getLoc(), divOp.getLhs(), divOp.getRhs());
        divOp.replaceAllUsesWith(mulOp.getResult());
        divOp->erase();
#endif
        return;
      }
    });
  }
};

int main(int argc, char **argv) {
  // This registration is making the pass accessible from the command line.
  PassRegistration<MyPass>();

  DialectRegistry registry;
  registry.insert<func::FuncDialect, arith::ArithDialect>();
  return asMainReturnCode(
      MlirOptMain(argc, argv, "Pass Management Driver\n", registry));

  return 0;
}
