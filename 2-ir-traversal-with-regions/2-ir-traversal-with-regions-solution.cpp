#include <mlir/IR/BuiltinAttributes.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/OwningOpRef.h>
#include <mlir/Parser/Parser.h>

#include <llvm/Support/raw_ostream.h>

#include <iostream>

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
  llvm::errs() << indent() << "- Operation name: " << op->getName() << "\n";

  // Print regions here
  llvm::errs() << indent() << "- has " << op->getNumRegions() << " regions\n";
  for (auto &region : op->getRegions()) {
    IndentRAII pushIndentation;
    // Note: size() is O(N), we need to traverse a linked list...
    llvm::errs() << indent() << "- Region has "
                 << std::distance(region.begin(), region.end()) << " blocks\n";
    for (Block &block : region) {
      IndentRAII pushIndentation;
      // Note: size() is O(N), we need to traverse a linked list...
      llvm::errs() << indent() << "- Block has " << block.getNumArguments()
                   << " arguments and "
                   << std::distance(block.begin(), block.end())
                   << " operations\n";
      for (Operation &op : block) {
        IndentRAII pushIndentation;
        printOperation(&op);
      }
    }
  }
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

    // TODO: try to use the op.walk(...) API instead!

    printOperation(&op);
  }

  return 0;
}
