//=== MLModelRunner/Utils/DataTypes.h - Supported Data Types - C++ -------===//
//
// Part of the MLCompilerBridge Project, under the Apache License v2.0 with LLVM
// Exceptions. See the LICENSE file for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file defines the bit widths of integral and floating point types
/// supported by the MLCompilerBridge.
///
/// The bit widths of floating and integral types supported by the
/// MLCompilerBridge can be configured by defining the MLBRIDGE_EXTENDED_TYPES
/// macro in the CMakeLists.txt file.
///
//===----------------------------------------------------------------------===//

#ifndef MLBRIDGE_DATATYPES_H
#define MLBRIDGE_DATATYPES_H

namespace MLBridge {

#ifdef MLBRIDGE_EXTENDED_TYPES
using RealType = double;
using IntegerType = long;
#else
using RealType = float;
using IntegerType = int;
#endif

} // namespace MLBridge

#endif
