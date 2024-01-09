from abc import ABC, abstractmethod
from BaseCompilerInterface import BaseCompilerInterface
import os
import io

import sys
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
    def __init__(self, mode, stub_class=None, hostip='127.0.0.1', hostport=50051, add_server_method=None,grpc_service_obj=None ):
        super().__init__('protobuf')
        self.mode = mode
        self.host=hostip
        self.server_port = hostport

        if self.mode == 'client':
            self.channel = grpc.insecure_channel('{}:{}'.format(self.host,self.server_port))
            print("Setting stub", stub_class)
            self.stub = stub_class(self.channel)
        
        elif self.mode == 'server':
            self.grpc_service_obj = grpc_service_obj
            self.add_server_method = add_server_method
            self.start_server()

    def __del__(self):
        pass

    ## Sends query to compiler and returns deserialized result.
    def evaluate(self, mode = None):
        out = self.serdes_obj.getOutputBuffer()
        return self.stub.queryCompiler(out)


    ## Starts gRPC server
    def start_server(self):
        if self.mode == 'server':
            server=grpc.server(futures.ThreadPoolExecutor(max_workers=20),options = [
                        ('grpc.max_send_message_length', 200*1024*1024), #50MB
                                ('grpc.max_receive_message_length', 200*1024*1024) #50MB
                                    ])

            self.add_server_method(self.grpc_service_obj,server)
            server.add_insecure_port('{}:{}'.format(self.host,self.server_port))

            server.start()
            print("Server Running")        
            server.wait_for_termination()

