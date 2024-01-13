#include "MLModelRunner/Utils/MLConfig.h"

llvm::cl::opt<std::string> MLBridge::MLConfig::mlconfig(
    "ml-config-path", llvm::cl::Hidden, llvm::cl::Optional,
    llvm::cl::desc("Path to ML config files"), llvm::cl::init(""));
