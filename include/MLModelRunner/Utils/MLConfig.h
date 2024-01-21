//=== MLModelRunner/Utils/MLConfig.h -MLConfig class definition -*- C++ -*-===//
//
// Part of the MLCompilerBridge Project, under the Apache License v2.0 with LLVM
// Exceptions. See the LICENSE file for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file defines the MLConfig class, which is a wrapper around the MLConfig
/// command line option for passing information like path of the models and
/// other configuration to the compiler passes.
///
//===----------------------------------------------------------------------===//

#ifndef MLBRIDGE_CONFIG_H
#define MLBRIDGE_CONFIG_H

#include "llvm/Support/CommandLine.h"

namespace MLBridge {
namespace MLConfig {
extern llvm::cl::opt<std::string> mlconfig;
} // namespace MLConfig
} // namespace MLBridge

#endif
