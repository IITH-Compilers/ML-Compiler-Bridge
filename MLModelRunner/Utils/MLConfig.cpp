//===- MLConfig.cpp - Set ML Config Paths -----------------------*- C++ -*-===//
//
// Part of the MLCompilerBridge Project, under the Apache License v2.0 with LLVM
// Exceptions. See the LICENSE file for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MLModelRunner/Utils/MLConfig.h"

llvm::cl::opt<std::string> MLBridge::MLConfig::mlconfig(
    "ml-config-path", llvm::cl::Hidden, llvm::cl::Optional,
    llvm::cl::desc("Path to ML config files"), llvm::cl::init(""));
