# ------------------------------------------------------------------------------
#
# Part of the MLCompilerBridge Project, under the Apache License v2.0 with LLVM
# Exceptions. See the LICENSE file for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# ------------------------------------------------------------------------------
##
## @file
## Compiler interface for pipes. This class implements methods for communication
## with compiler using pipes.
##
# ------------------------------------------------------------------------------

from .BaseCompilerInterface import BaseCompilerInterface
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
        self.tc_buffer = None
        self.fc_buffer = None
        self.buffer = None
        self.init_pipes()

    def __del__(self):
        self.close_pipes()
        self.remove_pipes()

    ## Sends query to compiler and returns deserialized result.
    def evaluate(self, mode=None):
        out = self.serdes_obj.getOutputBuffer()
        if out is not None:
            self.tc_buffer.write(out)
            self.tc_buffer.flush()

        if mode == "exit":
            return None

        result = self.serdes_obj.deserializeData(self.fc_buffer)

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
        self.tc_buffer = io.BufferedWriter(io.FileIO(self.to_compiler, "wb"))
        self.fc_buffer = io.BufferedReader(io.FileIO(self.from_compiler, "rb"))

    ## Closes the buffered reader/writers.
    def close_pipes(self):
        if self.fc_buffer is not None:
            self.tc_buffer.close()
            self.fc_buffer.close()
            self.tc_buffer = None
            self.fc_buffer = None

    ## Deletes the pipe files.
    def remove_pipes(self):
        os.remove(self.to_compiler)
        os.remove(self.from_compiler)
