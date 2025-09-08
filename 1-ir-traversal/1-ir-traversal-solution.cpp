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
  llvm::errs() << "  - Operation operands: " << op->getNumOperands() << "\n";
  for (Value operand : op->getOperands()) {
    llvm::errs() << " Operand " << operand << "\n";
  }
  llvm::errs() << "  - Operation results: " << op->getNumResults() << "\n";
  for (OpResult result : op->getResults()) {
    auto uses = result.getUses();
    llvm::errs() << " Result " << result.getResultNumber() << " " << result
                 << " has " << std::distance(uses.begin(), uses.end())
                 << " uses\n";
  }
  DictionaryAttr discardableAttrs = op->getDiscardableAttrDictionary();
  if (discardableAttrs.empty()) {
    llvm::errs() << "  - Operation has no discardable attributes\n";
  } else {
    for (NamedAttribute discardableAttr : op->getDiscardableAttrs()) {
      StringAttr name = discardableAttr.getName();
      Attribute value = discardableAttr.getValue();
      llvm::errs() << "    - " << name << " : " << value << "\n";
    }
  }
  // Inherent attribute are stored rather opaquely
  Attribute properties = op->getPropertiesAsAttribute();
  llvm::errs() << "  - Operation properties: " << properties << "\n";

  // This is only relevant to build a CFG: operations can have block
  // successors.
  llvm::errs() << "  - Operation has " << op->getNumSuccessors()
               << " successors\n";
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
