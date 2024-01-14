//=== MLModelRunner/Utils/MLConfig.h - MLConfig class definition ---*- C++ -*-===//
//
// Part of the MLCompilerBridge Project
//
//===------------------===//

#ifndef MLBRIDGE_CONFIG_H
#define MLBRIDGE_CONFIG_H

#include "llvm/Support/CommandLine.h"

namespace MLBridge {
class MLConfig {
public:
  static llvm::cl::opt<std::string> mlconfig;
};
} // namespace MLBridge

#endif
