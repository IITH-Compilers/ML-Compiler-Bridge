# ------------------------------------------------------------------------------
#
# Part of the MLCompilerBridge Project, under the Apache License v2.0 with LLVM
# Exceptions. See the LICENSE file for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# ------------------------------------------------------------------------------
##
## @file
## SerDes for JSON and bitstream data.
##
# ------------------------------------------------------------------------------

import json
from . import log_reader
import ctypes
import struct


class NpEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, ctypes.c_long):
            return obj.value
        if isinstance(obj, ctypes.c_double):
            return obj.value
        return super(NpEncoder, self).default(obj)


## Class for serialization and deserialization in various formats for communication.
class SerDes:
    ## Contructor for SerDes object
    # @param data_format Format for serialization and deserialization
    def __init__(self, data_format):
        self.buffer = None
        self.data_format = data_format
        self.read_stream_iter = None

        self.serMap = {
            "json": self.serializeJson,
            "bytes": self.serializeBytes,
            "protobuf": self.serializeProtobuf,
        }
        self.desMap = {
            "json": self.deserializeJson,
            "bytes": self.deserializeBytes,
            "protobuf": self.deserializeProtobuf,
        }

    ##  Deserializes data for specified data format
    # @param rawdata Datastream receiving serialized data
    # @return Deserialized data
    def deserializeData(self, rawdata):
        return self.desMap[self.data_format](rawdata)

    ##  Deserializes and returns JSON data
    def deserializeJson(self, datastream):
        hdr = datastream.read(8)
        size = int.from_bytes(hdr, "little")
        data = datastream.read(size)
        return json.loads(data)

    ##  Deserializes and returns bitstream data
    def deserializeBytes(self, datastream):
        if self.read_stream_iter is None:
            self.read_stream_iter = log_reader.read_stream2(
                datastream
            )  # try to make it indep
        hdr = datastream.read(8)
        context, observation_id, features, score = next(self.read_stream_iter)
        return features

    # Not implemented
    def deserializeProtobuf(self, datastream):
        raise NotImplementedError

    ## Serializes data and places it in a buffer
    def serializeData(self, data):
        self.serMap[self.data_format](data)

    ## Serializes data to JSON
    def serializeJson(self, data):
        msg = json.dumps({"out": data}, cls=NpEncoder).encode("utf-8")
        hdr = len(msg).to_bytes(8, "little")
        self.buffer = hdr + msg

    ## Serializes data to bitstream
    def serializeBytes(self, data):
        def _pack(data):
            if isinstance(data, int):
                return struct.pack("i", data)
            elif isinstance(data, float):
                return struct.pack("f", data)
            elif isinstance(data, str) and len(data) == 1:
                return struct.pack("c", data)
            elif isinstance(data, ctypes.c_double):
                return struct.pack("d", data.value)
            elif isinstance(data, ctypes.c_long):
                return struct.pack("l", data.value)
            elif isinstance(data, list):
                return b"".join([_pack(x) for x in data])

        msg = _pack(data)
        hdr = len(msg).to_bytes(8, "little")
        self.buffer = hdr + msg

    # Implemented outside for now
    def serializeProtobuf(self, data):
        self.buffer = data

    ## Returns value in buffer and empties it
    # @return Data from output buffer
    def getOutputBuffer(self):
        out = self.buffer
        self.buffer = None
        return out
