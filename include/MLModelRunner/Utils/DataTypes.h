//=== MLModelRunner/Utils/DataTypes.h - Supported Data Types - C++ -------===//
//
// Part of the MLCompilerBridge Project, under the Apache 2.0 License.
// See the LICENSE file under home directory for license and copyright
// information.
//
//===----------------------------------------------------------------------===//
//
// This file defines the bit widths of integral and floating point types
// supported by the MLCompilerBridge.
//
// The bit widths of floating and integral types supported by the
// MLCompilerBridge can be configured by defining the MLBRIDGE_EXTENDED_TYPES
// macro in the CMakeLists.txt file.
//
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
