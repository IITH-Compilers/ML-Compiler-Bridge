# ------------------------------------------------------------------------------
#
# Part of the MLCompilerBridge Project, under the Apache License v2.0 with LLVM
# Exceptions. See the LICENSE file for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# ------------------------------------------------------------------------------
import argparse
import numpy as np
import ctypes

import sys
import os
import torch, torch.nn as nn
import torch.onnx
import subprocess
import time

BUILD_DIR = "../build_release"
sys.path.extend(
    [
        f"{BUILD_DIR}/MLModelRunner/gRPCModelRunner/Python-Utilities",
    ]
)
from compilerinterface import PipeCompilerInterface, GrpcCompilerInterface

FAIL = 1
SUCCESS = 0

parser = argparse.ArgumentParser()
parser.add_argument("--use_pipe", default=False, help="Use pipe or not", required=False)
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
    "--silent", type=bool, help="Only prints errors when set to true", default=False
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
)
parser.add_argument(
    "--test_number",
    type=int,
    help="Datatype number for test",
    default=0,
)
parser.add_argument(
    "--export_onnx",
    help="Export onnx test model",
    required=False,
    default=False,
)
args = parser.parse_args()

if args.test_number <= 1:
    import MLBridgeTest_int_pb2, MLBridgeTest_int_pb2_grpc

    MLBridgeTest_pb2, MLBridgeTest_pb2_grpc = (
        MLBridgeTest_int_pb2,
        MLBridgeTest_int_pb2_grpc,
    )
elif args.test_number == 2:
    import MLBridgeTest_long_pb2, MLBridgeTest_long_pb2_grpc

    MLBridgeTest_pb2, MLBridgeTest_pb2_grpc = (
        MLBridgeTest_long_pb2,
        MLBridgeTest_long_pb2_grpc,
    )
elif args.test_number == 3:
    import MLBridgeTest_float_pb2, MLBridgeTest_float_pb2_grpc

    MLBridgeTest_pb2, MLBridgeTest_pb2_grpc = (
        MLBridgeTest_float_pb2,
        MLBridgeTest_float_pb2_grpc,
    )
elif args.test_number == 4:
    import MLBridgeTest_double_pb2, MLBridgeTest_double_pb2_grpc

    MLBridgeTest_pb2, MLBridgeTest_pb2_grpc = (
        MLBridgeTest_double_pb2,
        MLBridgeTest_double_pb2_grpc,
    )
elif args.test_number == 5:
    import MLBridgeTest_char_pb2, MLBridgeTest_char_pb2_grpc

    MLBridgeTest_pb2, MLBridgeTest_pb2_grpc = (
        MLBridgeTest_char_pb2,
        MLBridgeTest_char_pb2_grpc,
    )
elif args.test_number == 6:
    import MLBridgeTest_bool_pb2, MLBridgeTest_bool_pb2_grpc

    MLBridgeTest_pb2, MLBridgeTest_pb2_grpc = (
        MLBridgeTest_bool_pb2,
        MLBridgeTest_bool_pb2_grpc,
    )
elif args.test_number == 7:
    import MLBridgeTest_vec_int_pb2, MLBridgeTest_vec_int_pb2_grpc

    MLBridgeTest_pb2, MLBridgeTest_pb2_grpc = (
        MLBridgeTest_vec_int_pb2,
        MLBridgeTest_vec_int_pb2_grpc,
    )
elif args.test_number == 8:
    import MLBridgeTest_vec_long_pb2, MLBridgeTest_vec_long_pb2_grpc

    MLBridgeTest_pb2, MLBridgeTest_pb2_grpc = (
        MLBridgeTest_vec_long_pb2,
        MLBridgeTest_vec_long_pb2_grpc,
    )
elif args.test_number == 9:
    import MLBridgeTest_vec_float_pb2, MLBridgeTest_vec_float_pb2_grpc

    MLBridgeTest_pb2, MLBridgeTest_pb2_grpc = (
        MLBridgeTest_vec_float_pb2,
        MLBridgeTest_vec_float_pb2_grpc,
    )
elif args.test_number == 10:
    import MLBridgeTest_vec_double_pb2, MLBridgeTest_vec_double_pb2_grpc

    MLBridgeTest_pb2, MLBridgeTest_pb2_grpc = (
        MLBridgeTest_vec_double_pb2,
        MLBridgeTest_vec_double_pb2_grpc,
    )


class DummyModel(nn.Module):
    def __init__(self):
        nn.Module.__init__(self)

    def forward(self, input):
        return 2 - input


def export_onnx_model(input_dim=100):
    onnx_filename = "./onnx/dummy_model.onnx"
    dummy_value = torch.randn(1, input_dim)
    torch.onnx.export(
        DummyModel(),
        dummy_value,
        onnx_filename,
        input_names=["obs"],
        verbose=True,
        export_params=True,
    )
    print(f"Model exported to {onnx_filename}")


expected_type = {
    1: "int",
    2: "long",
    3: "float",
    4: "double",
    5: "char",
    6: "bool",
    7: "vec_int",
    8: "vec_long",
    9: "vec_float",
    10: "vec_double",
}

expected_data = {
    1: 11,
    2: 1234567890,
    3: 3.14,
    4: 0.123456789123456789,
    5: ord("a"),
    6: True,
    7: [11, 22, 33],
    8: [123456780, 222, 333],
    9: [11.1, 22.2, 33.3],
    10: [-1.1111111111, -2.2222222222, -3.3333333333],
}

returned_data = {
    1: 12,
    2: 1234567891,
    3: 4.14,
    4: 1.123456789123456789,
    5: ord("b"),
    6: False,
    7: [12, 23, 34],
    8: [123456780, 123456781, 123456782],
    9: [1.11, 2.22, -3.33, 0],
    10: [1.12345678912345670, -1.12345678912345671],
}

if args.use_pipe and args.data_format == "bytes":
    returned_data.update(
        {
            2: ctypes.c_long(1234567891),
            4: ctypes.c_double(1.123456789123456789),
            8: [
                ctypes.c_long(123456780),
                ctypes.c_long(123456781),
                ctypes.c_long(123456782),
            ],
            10: [
                ctypes.c_double(1.12345678912345670),
                ctypes.c_double(-1.12345678912345671),
            ],
        }
    )
if args.use_pipe and args.data_format == "json":
    returned_data.update(
        {
            2: ctypes.c_long(12345),
            4: ctypes.c_double(1.123456789123456789),
            8: [ctypes.c_long(6780), ctypes.c_long(6781), ctypes.c_long(6782)],
            10: [
                ctypes.c_double(1.12345678912345670),
                ctypes.c_double(-1.12345678912345671),
            ],
        }
    )

status = SUCCESS

# test index vs received data
def checkData(index, data):
    global status
    if not args.silent:
        print(" ", expected_type[index], "request:", data)

    if isinstance(expected_data[index], list):
        for e, d in zip(expected_data[index], data):
            if abs(e - d) > 10e-6:
                print(
                    f"Error: Expected {expected_type[index]} request: {expected_data[index]}, Received: {data}"
                )
                status = FAIL
                # raise Exception(f"Mismatch in {expected_type[i]}")

    elif abs(data - expected_data[index]) > 10e-6:
        print(
            f"Error: Expected {expected_type[index]} request: {expected_data[index]}, Received: {data}"
        )
        status = FAIL
        # raise Exception(f"Mismatch in {expected_type[i]}")


def run_pipe_communication(data_format, pipe_name):
    compiler_interface = PipeCompilerInterface(data_format, "./" + pipe_name)
    if not args.silent:
        print("PipeCompilerInterface init...")
    compiler_interface.reset_pipes()

    i = 0
    while True:
        i += 1
        try:
            data = compiler_interface.evaluate()
            if data_format == "json":
                key = list(data)[0]
                data = data[key]
            elif data_format == "bytes":
                data = [x for x in data[0]]
                if len(data) == 1:
                    data = data[0]

            checkData(i, data)

            compiler_interface.populate_buffer(returned_data[i])

            if i == len(expected_type):
                data = compiler_interface.evaluate(mode="exit")
                exit(status)
        except Exception as e:
            print("*******Exception*******", e)
            compiler_interface.reset_pipes()


class service_server(MLBridgeTest_pb2_grpc.MLBridgeTestService):
    def __init__(self):
        pass

    def getAdvice(self, request, context):
        try:
            request_type = [var for var in dir(request) if "request" in var]
            data = getattr(request, request_type[0])
            checkData(args.test_number, data)
            if status == FAIL:
                os.system("touch mlbridge-grpc-fail.txt")
            reply = MLBridgeTest_pb2.Reply(action=returned_data[args.test_number])
            return reply
        except:
            reply = MLBridgeTest_pb2.Reply(action=-1)
            return reply


def run_grpc_communication():
    # parent with test_number 0 spawns different servers
    if args.test_number == 0:
        process_list = []
        for i in range(1, len(expected_type) + 1):
            p = subprocess.Popen(
                f"python mlbridge-test.py --use_grpc --server_port={args.server_port} --silent={args.silent} --test_number={i}".split(),
            )
            process_list.append(p)

        time.sleep(10)
        global status
        for p in process_list:
            if os.path.isfile("mlbridge-grpc-fail.txt"):
                status = FAIL
                os.system("rm mlbridge-grpc-fail.txt")
            p.terminate()
        exit(status)

    # servers serve different datatypes
    else:
        compiler_interface = GrpcCompilerInterface(
            mode="server",
            add_server_method=MLBridgeTest_pb2_grpc.add_MLBridgeTestServiceServicer_to_server,
            grpc_service_obj=service_server(),
            hostport=args.server_port + args.test_number,
        )
        compiler_interface.start_server()


if __name__ == "__main__":
    if args.use_pipe:
        run_pipe_communication(args.data_format, args.pipe_name)
    elif args.use_grpc:
        run_grpc_communication()
    elif args.export_onnx:
        export_onnx_model()
