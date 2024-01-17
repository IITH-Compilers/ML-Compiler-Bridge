//=== MLModelRunner/Utils/Debug.h - Debug definitions with support - C++ --===//
//
// Part of the MLCompilerBridge Project, under the Apache License v2.0 with LLVM
// Exceptions. See the LICENSE file for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file defines the debug macros for the MLCompilerBridge.
///
//===----------------------------------------------------------------------===//

#ifndef MLBRIDGE_DEBUG_H
#define MLBRIDGE_DEBUG_H

namespace MLBridge {

#ifdef DEBUG_MODE
#define MLBRIDGE_DEBUG(X)                                                      \
  do {                                                                         \
    X;                                                                         \
  } while (false)
#else
#define MLBRIDGE_DEBUG(X)
#endif

} // namespace MLBridge

#endif
