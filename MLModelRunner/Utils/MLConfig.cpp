#include "MLModelRunner/Utils/MLConfig.h"

llvm::cl::opt<std::string>
    llvm::MLConfig::mlconfig("ml-config-path", cl::Hidden, cl::Optional,
                             cl::desc("Path to ML config files"), cl::init(""));
