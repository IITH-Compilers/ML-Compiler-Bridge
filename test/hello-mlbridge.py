# ------------------------------------------------------------------------------
#
# Part of the MLCompilerBridge Project, under the Apache License v2.0 with LLVM
# Exceptions. See the LICENSE file for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# ------------------------------------------------------------------------------

import argparse
import sys
import torch, torch.nn as nn

sys.path.append("../CompilerInterface")
from PipeCompilerInterface import PipeCompilerInterface
from GrpcCompilerInterface import GrpcCompilerInterface

sys.path.append("../MLModelRunner/gRPCModelRunner/Python-Utilities")
import helloMLBridge_pb2, helloMLBridge_pb2_grpc, grpc
from concurrent import futures

parser = argparse.ArgumentParser()
parser.add_argument(
    "--use_pipe", type=bool, default=False, help="Use pipe or not", required=False
)
parser.add_argument(
    "--data_format",
    type=str,
    choices=["json", "protobuf", "bytes"],
    help="Data format to use for communication",
)
parser.add_argument(
    "--pipe_name",
    type=str,
    help="Pipe Name",
)
parser.add_argument(
    "--use_grpc",
    action="store_true",
    help="Use grpc communication",
    required=False,
    default=False,
)
parser.add_argument(
    "--server_port",
    type=int,
    help="Server Port",
    default=5050,
)
args = parser.parse_args()


class DummyModel(nn.Module):
    def __init__(self, input_dim=10):
        nn.Module.__init__(self)
        self.fc1 = nn.Linear(input_dim, 1)

    def forward(self, input):
        x = self.fc1(input)
        return x


def run_pipe_communication(data_format, pipe_name):
    compiler_interface = PipeCompilerInterface(data_format, "/tmp/" + pipe_name)
    print("PipeCompilerInterface init...")
    compiler_interface.reset_pipes()

    while True:
        try:
            data = compiler_interface.evaluate()
            if data_format == "json":
                data = data["tensor"]
                # print("Data: ", data["tensor"])
            elif data_format == "bytes":
                data = [x for x in data[0]]
                # print("Data: ", [x for x in data[0]])
            print("data: ", data)
            model = DummyModel(input_dim=len(data))
            action = model(torch.Tensor(data))
            compiler_interface.populate_buffer(3)
        except Exception as e:
            print("*******Exception*******", e)
            compiler_interface.reset_pipes()


class service_server(helloMLBridge_pb2_grpc.HelloMLBridgeService):
    def __init__(self):
        # self.serdes = SerDes.SerDes(data_format, pipe_name)
        # self.serdes.init()
        pass

    def getAdvice(self, request, context):
        try:
            print("Entered getAdvice")
            print("Data: ", request.tensor)
            reply = helloMLBridge_pb2.ActionRequest(action=1)
            return reply
        except:
            reply = helloMLBridge_pb2.ActionRequest(action=-1)
            return reply


def test_func():
    data = 3.24
    import struct

    print(data, type(data))
    byte_data = struct.pack("f", data)
    print(byte_data, len(byte_data))

    print("decoding...")
    decoded = float(byte_data)

    print(decoded, type(decoded))


if __name__ == "__main__":
    if args.use_pipe:
        run_pipe_communication(args.data_format, args.pipe_name)
    elif args.use_grpc:
        compiler_interface = GrpcCompilerInterface(
            mode="server",
            add_server_method=helloMLBridge_pb2_grpc.add_HelloMLBridgeServiceServicer_to_server,
            grpc_service_obj=service_server(),
            hostport=args.server_port,
        )
        compiler_interface.start_server()
