import os, io
import json
import log_reader


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
            self.read_stream_iter = log_reader.read_stream2(datastream)        # try to make it indep
        hdr = datastream.read(8)
        context, observation_id, features, score = next(self.read_stream_iter)
        return features
    
    # Not implemented
    def deserializeProtobuf(self, datastream):
        raise NotImplementedError

    ## Serializes data and places it in a buffer
    # @param data
    def serializeData(self, data):
        self.serMap[self.data_format](data)

    ## Serializes data to JSON 
    # @param data
    def serializeJson(self, data):
        msg = json.dumps({"out": data}).encode("utf-8")
        hdr = len(msg).to_bytes(8, "little")
        self.buffer = hdr + msg

    ## Serializes data to bitstream
    # @param data
    def serializeBytes(self, data):
        if isinstance(data, list):
            msg = b"".join([x.to_bytes(4, "little", signed=True) for x in data])
        else:
            msg = data.to_bytes(4, "little", signed=True)
        hdr = len(msg).to_bytes(8, "little")
        self.buffer = hdr + msg

    # Implemented outside for now
    def serializeProtobuf(self, data):
        self.buffer = data

    ## Returns value in buffer and empties it
    # @return data from output buffer 
    def getOutputBuffer(self):
        out = self.buffer
        self.buffer = None
        return out
