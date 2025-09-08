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

int indentationLevel = 0;
struct IndentRAII {
  IndentRAII() { ++indentationLevel; }
  ~IndentRAII() { --indentationLevel; }
};
// Manage indentation in a stream.
struct indent {};
llvm::raw_ostream &operator<<(llvm::raw_ostream &os, indent tag) {
  for (int l = 0; l < indentationLevel; ++l)
    os << "  ";
  return os;
}

void printOperation(Operation *op) {
  // References:
  // mlir/IR/Operation.h

  llvm::errs() << indent() << "================================\n";
  llvm::errs() << indent() << "Visiting operation: "
               << OpWithFlags(op, OpPrintingFlags().skipRegions()) << "\n";
  IndentRAII pushIndentation;
  LDBG(2) << indent() << "- Operation name: " << op->getName();
  for (Value operand : op->getOperands()) {
    LDBG(3) << " Operand " << operand;
  }
  LDBG(3) << "  - Operation results: " << op->getNumResults();
  for (OpResult result : op->getResults()) {
    auto uses = result.getUses();
    LDBG(4) << " Result " << result.getResultNumber() << " " << result
            << " has " << std::distance(uses.begin(), uses.end()) << " uses";
  }
}

class MyPass : public PassWrapper<MyPass, OperationPass<>> {
public:
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(MyPass)
  // This is how we'll refer to the pass on the command line.
  StringRef getArgument() const override { return "my-pass"; }
  StringRef getDescription() const override {
    return "This pass is printing the operation recursively";
  }

  void runOnOperation() override {
    llvm::errs() << "Pass is running\n";
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
    Block &blockBody = regionBody.getBlocks().front();
    for (Operation &op : blockBody) {
      printOperation(&op);
    }
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
