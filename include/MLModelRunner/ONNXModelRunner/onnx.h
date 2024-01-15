//=== MLModelRunner/ONNXModelRunner/onnx.h --- ONNX C++ Interface -----===//
//
// Part of the MLCompilerBridge Project, under the Apache 2.0 License.
// See the LICENSE file under home directory for license and copyright
// information.
//
//===---------------------------------------------------------------------===//
//
// This file defines the ONNXModel class, which is a wrapper around the ONNX
// C++ interface.
//
//===---------------------------------------------------------------------===//

#ifndef ONNX_MODELRUNNER_ONNX_H
#define ONNX_MODELRUNNER_ONNX_H

#include "llvm/ADT/SmallVector.h"

#include <cstdint>
#include <vector>

namespace Ort {
class Env;
class Session;
} // namespace Ort

class ONNXModel {
  Ort::Env *env;
  const char *model_path;
  Ort::Session *session;

public:
  ONNXModel(const char *model_path);

  /// Runs the ONNX model on the input and returns the output
  void run(llvm::SmallVector<float, 100> &input,
           llvm::SmallVector<float, 100> &output);
};

#endif // ONNX_MODELRUNNER_ONNX_H
