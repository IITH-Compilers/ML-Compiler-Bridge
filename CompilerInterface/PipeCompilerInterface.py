from abc import ABC, abstractmethod
from BaseCompilerInterface import BaseCompilerInterface
import os
import io

## This class implements methods for communication with compiler using pipes.
class PipeCompilerInterface(BaseCompilerInterface):
    ## Initializes PipeCompilerInterface object.
    # @param data_format Data format for serialization
    # @param pipe_name Name for pipe file
    def __init__(self, data_format=None, pipe_name=None):
        super().__init__(data_format)
        self.pipe_name = pipe_name
        self.to_compiler = None
        self.from_compiler = None
        self.tc = None
        self.fc = None
        self.buffer = None
        self.init_pipes()

    def __del__(self):
        self.close_pipes()
        self.remove_pipes()

    ## Sends query to compiler and returns deserialized result.
    def evaluate(self, mode=None):
        out = self.serdes_obj.getOutputBuffer()
        if out is not None:
            self.tc.write(out)
            self.tc.flush()

        if mode == "exit":
            return None

        result = self.serdes_obj.deserializeData(self.fc)

        return result

    ## Creates pipe files for communication.
    def init_pipes(self):
        self.to_compiler = self.pipe_name + ".in"
        self.from_compiler = self.pipe_name + ".out"
        if os.path.exists(self.to_compiler):
            os.remove(self.to_compiler)
        if os.path.exists(self.from_compiler):
            os.remove(self.from_compiler)

        os.mkfifo(self.to_compiler, 0o666)
        os.mkfifo(self.from_compiler, 0o666)

    ## Resets the buffered reader/writers.
    def reset_pipes(self):
        self.tc = io.BufferedWriter(io.FileIO(self.to_compiler, "wb"))
        self.fc = io.BufferedReader(io.FileIO(self.from_compiler, "rb"))

    ## Closes the buffered reader/writers.
    def close_pipes(self):
        if self.fc is not None:
            self.tc.close()
            self.fc.close()
            self.tc = None
            self.fc = None

    ## Deletes the pipe files.
    def remove_pipes(self):
        os.remove(self.to_compiler)
        os.remove(self.from_compiler)
