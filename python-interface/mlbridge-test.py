import log_reader
import argparse
import os, io, json
import SerDes
import ctypes
import numpy as np

import sys
import torch, torch.nn as nn

sys.path.append(
    "../MLModelRunner/gRPCModelRunner/Python-Utilities"
)
import helloMLBridge_pb2, helloMLBridge_pb2_grpc, grpc
from concurrent import futures

parser = argparse.ArgumentParser()
parser.add_argument("--use_pipe", type=bool, default=False, help="Use pipe or not", required=False)
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
    

expected_type = {
    1: 'int',
    2: 'long',
    3: 'float',
    4: 'double',
    5: 'char',
    6: 'bool',
    7: 'vec_int',
    8: 'vec_long',
    9: 'vec_float',
    10: 'vec_double'
}

expected_data = {
    1: 11,
    2: 1234567890,
    3: 3.14,
    4: 0.123456789123456789,
    5: ord('a'),
    6: True,
    7: [11,22,33],
    8: [111,222,333],
    9: [11.1,22.2,33.3],
    10: [-1.1111111111,-2.2222222222,-3.3333333333],
}

returned_data = {
    1: 12,
    2: ctypes.c_long(1234567891),
    3: 4.14,
    4: ctypes.c_double(1.123456789123456789),
    5: ord('b'),
    6: False,
    7: [12,23,34],
    8: [ctypes.c_long(123456780),ctypes.c_long(123456781),ctypes.c_long(123456782)],
    9: [1.11,2.22,-3.33,0],
    10: [ctypes.c_double(1.12345678912345670), ctypes.c_double(-1.12345678912345671)]
}

def run_pipe_communication(data_format, pipe_name):
    serdes = SerDes.SerDes(data_format, "/tmp/" + pipe_name)
    print('Serdes init...')
    serdes.init()
    i = 0
    while True:
        i += 1 
        try:
            data = serdes.readObservation()
            if data_format == "json":
                key = list(data)[0]
                data = data[key]
            elif data_format == "bytes":
                data = [x for x in data[0]]
                if len(data) == 1:
                    data = data[0]

            print(expected_type[i], "request:", data)

            if isinstance(expected_data[i], list):
                for e,d in zip(expected_data[i],data):
                    if abs(e-d)>10e-6:
                        print(f"Mismatch in {expected_type[i]}")
                        # raise Exception(f"Mismatch in {expected_type[i]}")

            elif abs(data - expected_data[i]) >10e-6: 
                print(f"Mismatch in {expected_type[i]}")
                # raise Exception(f"Mismatch in {expected_type[i]}")

            serdes.sendData(returned_data[i])
        except Exception as e:
            print("*******Exception*******", e)
            serdes.init()

class service_server(helloMLBridge_pb2_grpc.HelloMLBridgeService):
    def __init__(self, data_format, pipe_name):
        # self.serdes = SerDes.SerDes(data_format, pipe_name)
        # self.serdes.init()
        pass
    def getAdvice(self, request, context):
        try:
            print(request)
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
    byte_data = struct.pack('f', data)
    print(byte_data, len(byte_data))
    
    
    print('decoding...')
    decoded = float(byte_data)
    
    print(decoded, type(decoded))

if __name__ == "__main__":
    # test_func()
    # exit(0)
    if args.use_pipe:
        run_pipe_communication(args.data_format, args.pipe_name)
    elif args.use_grpc:
        server = grpc.server(
            futures.ThreadPoolExecutor(max_workers=20),
            options=[
                ("grpc.max_send_message_length", 200 * 1024 * 1024),
                ("grpc.max_receive_message_length", 200 * 1024 * 1024),
            ],
        )
        helloMLBridge_pb2_grpc.add_HelloMLBridgeServiceServicer_to_server(
            service_server(args.data_format, args.pipe_name), server
        )
        server.add_insecure_port(f"localhost:{args.server_port}")
        server.start()
        print("Server Running")
        server.wait_for_termination()
