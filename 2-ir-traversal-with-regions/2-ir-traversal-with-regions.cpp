#include <mlir/IR/BuiltinAttributes.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/OwningOpRef.h>
#include <mlir/Parser/Parser.h>

#include <llvm/Support/raw_ostream.h>

#include <iostream>

using namespace mlir;

void printOperation(Operation *op) {
  // References:
  // mlir/IR/Operation.h

  llvm::errs() << "================================\n";
  llvm::errs() << "Visiting operation: "
               << OpWithFlags(op, OpPrintingFlags().skipRegions()) << "\n";
  llvm::errs() << "  - Operation name: " << op->getName() << "\n";

  // Print regions here
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <input file>" << std::endl;
    return 1;
  }

  MLIRContext context;
  context.allowUnregisteredDialects();
  OwningOpRef<ModuleOp> module = parseSourceFile<ModuleOp>(argv[1], &context);
  if (!module) {
    std::cerr << "Failed to parse input file" << std::endl;
    return 1;
  }

  // Now we can start to traverse the module
  Region &regionBody = module->getRegion();
  Block &blockBody = regionBody.getBlocks().front();
  for (Operation &op : blockBody) {
    printOperation(&op);
  }

  return 0;
}
