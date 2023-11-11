//===- PipeModelRunner.cpp - noop ML model runner   ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// A runner that communicates with an external agent via 2 file descriptors.
//===----------------------------------------------------------------------===//
#include "MLModelRunner/PipeModelRunner.h"
#include "MLModelRunner/MLModelRunner.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/MC/MCContext.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#define DEBUG_TYPE "pipe-model-runner"

using namespace llvm;

static cl::opt<bool> DebugReply(
    "interactive-model-runner-echo-reply", cl::init(false), cl::Hidden,
    cl::desc("The PipeModelRunner will echo back to stderr "
             "the data received from the host (for debugging purposes)."));

PipeModelRunner::PipeModelRunner(StringRef OutboundName, StringRef InboundName,
                                 BaseSerDes::Kind SerDesType, LLVMContext *Ctx)
    : MLModelRunner(Kind::Pipe, SerDesType, Ctx),
      InEC(sys::fs::openFileForRead(InboundName, Inbound)) {
  this->InboundName = InboundName.str();
  if (InEC) {
    if (this->Ctx)
      this->Ctx->emitError("Cannot open inbound file: " + InEC.message());
    return;
  }
  {
    OutStream = std::make_unique<raw_fd_ostream>(OutboundName, OutEC);
    if (OutEC) {
      if (this->Ctx)
        this->Ctx->emitError("Cannot open outbound file: " + OutEC.message());
      return;
    }
  }
}

PipeModelRunner::~PipeModelRunner() {
  // close the file descriptors
  sys::fs::file_t FDAsOSHandle = sys::fs::convertFDToNativeFile(Inbound);
  sys::fs::closeFile(FDAsOSHandle);

  OutStream->close();
}

std::string PipeModelRunner::readNBytes(size_t N) {
  std::string OutputBuffer(N, '\0');
  char *Buff = OutputBuffer.data();
  size_t InsPoint = 0;
  const size_t Limit = N;
  while (InsPoint < Limit) {
    auto ReadOrErr = ::sys::fs::readNativeFile(
        sys::fs::convertFDToNativeFile(Inbound),
        {Buff + InsPoint, OutputBuffer.size() - InsPoint});
    if (ReadOrErr.takeError()) {
      if (this->Ctx)
        this->Ctx->emitError("Failed reading from inbound file");
      break;
    }
    InsPoint += *ReadOrErr;
  }
  return OutputBuffer;
}

void PipeModelRunner::send(void *data) {
  // TODO: send data size first (a hack to send protbuf data properly)
  auto dataString = reinterpret_cast<std::string *>(data);
  size_t message_length = dataString->size();
  const char *message_length_ptr =
      reinterpret_cast<const char *>(&message_length);
  LLVM_DEBUG(errs() << "Message length: " << message_length << "\n");
  LLVM_DEBUG(errs() << "DataString.size(): " << dataString->size() << "\n");
  OutStream->write(message_length_ptr, sizeof(size_t));
  OutStream->write(dataString->data(), dataString->size());
  OutStream->flush();
}

void *PipeModelRunner::receive() {
  LLVM_DEBUG(errs() << "In PipeModelRunner receive...\n");
  auto hdr = readNBytes(8);
  LLVM_DEBUG(errs() << "Read header...\n");
  size_t MessageLength = 0;
  memcpy(&MessageLength, hdr.data(), sizeof(MessageLength));
  // Read message
  auto OutputBuffer = new std::string(readNBytes(MessageLength));
  LLVM_DEBUG(errs() << "OutputBuffer size: " << OutputBuffer->size() << "\n";
             errs() << "OutputBuffer: " << *OutputBuffer << "\n");

  return OutputBuffer;
}

void *PipeModelRunner::evaluateUntyped() {
  LLVM_DEBUG(llvm::errs() << "In PipeModelRunner evaluateUntyped...\n");
  auto *data = SerDes->getSerializedData();
  send(data);
  auto *reply = receive();
  LLVM_DEBUG(
      errs() << "In PipeModelRunner::evaluateUntyped() received data...\n");
  return SerDes->deserializeUntyped(reply);
}
