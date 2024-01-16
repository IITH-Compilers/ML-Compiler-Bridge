//=== MLModelRunner/ONNXModelRunner/utils.h - C++ ------------------------===//
//
// Part of the MLCompilerBridge Project, under the Apache 2.0 License.
// See the LICENSE file under home directory for license and copyright
// information.
//
//===----------------------------------------------------------------------===//

#ifndef ONNX_MODELRUNNER_UTILS_H
#define ONNX_MODELRUNNER_UTILS_H

#include "llvm/ADT/SmallVector.h"

namespace MLBridge {
typedef llvm::SmallVector<float, 300> Observation;

} // namespace MLBridge

#endif // ONNX_MODELRUNNER_UTILS_H
