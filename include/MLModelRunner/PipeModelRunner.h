//===- PipeModelRunner.h ---- "gym" ML model runner  -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//

#ifndef PipeModelRunner_H
#define PipeModelRunner_H

#include "MLModelRunner/MLModelRunner.h"
#include "SerDes/TensorSpec.h"
#include "SerDes/baseSerDes.h"
#include "llvm/Support/FileSystem.h"
#include <fstream>
#include <memory>
#include <system_error>

namespace MLBridge {

/// A MLModelRunner that asks for advice from an external agent, or host. It
/// uses 2 files - ideally named pipes - one to send data to that agent, and
/// one to receive advice.
/// The data exchange uses the training logger (Utils/TrainingLogger.h) format.
/// Specifically, the compiler will send the log header, set the context, and
/// send observations; the host is expected to reply with a tensor value after
/// each observation as a binary buffer that's conforming to the shape of the
/// advice. Interleaved, the data closely resembles the training log for a
/// log where we don't capture the reward signal.
///
/// Note that the correctness of the received data is the responsibility of the
/// host. In particular, if insufficient data were sent, the compiler will block
/// when waiting for an advice.
///
/// Note that the host can either open the pipes RW, or open first the pipe to
/// the compiler - i.e. the "Inbound" - and then the "Outbound", to avoid
/// deadlock. This is because the compiler first tries to open the inbound
/// (which will hang until there's a writer on the other end).
class PipeModelRunner : public MLModelRunner {
public:
  PipeModelRunner(llvm::StringRef OutboundName, llvm::StringRef InboundName,
                  BaseSerDes::Kind Kind, llvm::LLVMContext *Ctx = nullptr);

  static bool classof(const MLModelRunner *R) {
    return R->getKind() == MLModelRunner::Kind::Pipe;
  }

  void requestExit() override {}
  virtual ~PipeModelRunner();

private:
  void send(void *data);
  void *receive();
  void *evaluateUntyped() override;
  std::string readNBytes(size_t N);
  // This must be declared before InEC if we want to initialize it in the
  // ctor initializer list.
  std::string InboundName;
  int Inbound = -1;
  std::error_code OutEC;
  std::error_code InEC;
  std::unique_ptr<llvm::raw_fd_ostream> OutStream;
};
} // namespace MLBridge
#endif // PipeModelRunner_H
