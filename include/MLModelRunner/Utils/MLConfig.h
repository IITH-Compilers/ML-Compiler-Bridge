//=== MLModelRunner/Utils/MLConfig.h -MLConfig class definition -*- C++ -*-===//
//
// Part of the MLCompilerBridge Project, under the Apache 2.0 License.
// See the LICENSE file under home directory for license and copyright
// information.
//
//===----------------------------------------------------------------------===//
//
// This file defines the MLConfig class, which is a wrapper around the MLConfig
// command line option for passing information like path of the models and other
// configuration to the compiler passes.
//
//===----------------------------------------------------------------------===//

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
