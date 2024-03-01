# ------------------------------------------------------------------------------
#
# Part of the MLCompilerBridge Project, under the Apache License v2.0 with LLVM
# Exceptions. See the LICENSE file for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# ------------------------------------------------------------------------------
##
## @file
## Compiler interface for gRPC. This class implements methods for communication
## with compiler using gRPC.
##
# ------------------------------------------------------------------------------


from .BaseCompilerInterface import BaseCompilerInterface
import time

import grpc
from concurrent import futures

# requires grpc stub evaluate, service_server obj and adder method

## This class implements methods for communication with compiler using gRPC.
class GrpcCompilerInterface(BaseCompilerInterface):
    ## Initializes GrpcCompilerInterface object.
    # @param mode Can be 'client' or 'server'.
    # @param stub_class gRPC stub class used in 'client' mode
    # @param hostip
    # @param hostport
    # @param add_server_method used in 'server' mode
    # @param grpc_service_obj used in 'server' mode
    def __init__(
        self,
        mode,
        stub_class=None,
        hostip="127.0.0.1",
        hostport=50051,
        add_server_method=None,
        grpc_service_obj=None,
    ):
        super().__init__("protobuf")
        self.mode = mode
        self.host = hostip
        self.server_port = hostport

        if self.mode == "client":
            self.channel = grpc.insecure_channel(
                "{}:{}".format(self.host, self.server_port)
            )
            print("Setting stub", stub_class)
            self.stub = stub_class(self.channel)

        elif self.mode == "server":
            self.grpc_service_obj = grpc_service_obj
            self.add_server_method = add_server_method
            self.start_server()

    def __del__(self):
        pass

    ## Sends query to compiler and returns deserialized result.
    def evaluate(self, mode=None):
        out = self.serdes_obj.getOutputBuffer()
        return self.stub.queryCompiler(out)

    ## Starts gRPC server
    def start_server(self):
        if self.mode == "server":
            server = grpc.server(
                futures.ThreadPoolExecutor(max_workers=20),
                options=[
                    ("grpc.max_send_message_length", 200 * 1024 * 1024),  # 50MB
                    ("grpc.max_receive_message_length", 200 * 1024 * 1024),  # 50MB
                ],
            )

            self.add_server_method(self.grpc_service_obj, server)

            retries = 0
            max_retries = 30
            wait_seconds = 0.2
            retry_wait_backoff_exponent = 1.2

            while retries < max_retries:
                added_port = server.add_insecure_port(
                    "{}:{}".format(self.host, self.server_port)
                )

                if str(added_port) == str(self.server_port):
                    server.start()
                    print("Server Running")
                    server.wait_for_termination()
                    break
                else:
                    retries += 1
                    print(
                        "The port",
                        self.server_port,
                        "is already in use retrying! attempt: ",
                        retries,
                    )

                    time.sleep(wait_seconds)
                    wait_seconds *= retry_wait_backoff_exponent
