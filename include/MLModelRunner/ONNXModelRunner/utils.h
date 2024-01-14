//===----------------------------------------------------------------------===//
//
// Part of the ml-llvm-tools Project, under the BSD 4-Clause License.
// See the LICENSE.txt file under ml-llvm-tools directory for license
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
