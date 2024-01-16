//===- MLConfig.cpp - Set ML Config Paths -----------------------*- C++ -*-===//
//
// Part of the MLCompilerBridge Project, under the Apache 2.0 License.
// See the LICENSE file under home directory for license and copyright
// information.
//
//===----------------------------------------------------------------------===//

#include "MLModelRunner/Utils/MLConfig.h"

llvm::cl::opt<std::string> MLBridge::MLConfig::mlconfig(
    "ml-config-path", llvm::cl::Hidden, llvm::cl::Optional,
    llvm::cl::desc("Path to ML config files"), llvm::cl::init(""));
