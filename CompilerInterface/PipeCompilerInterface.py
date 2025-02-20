from abc import ABC, abstractmethod
from BaseCompilerInterface import BaseCompilerInterface
import os
import io
import select
import random

## This class implements methods for communication with compiler using pipes.
class PipeCompilerInterface(BaseCompilerInterface):
    
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

   
    def evaluate(self, mode=None):
       
        out = self.serdes_obj.getOutputBuffer()
        if out is not None:
            
            self.tc.write(out)
            self.tc.flush()  
        else:
            print("Empty output buffer in PipeCompilerInterface.")

        if mode == "exit":
            return None
        print("Before deserializing")        
        result = self.serdes_obj.deserializeData(self.fc)
        print("After deserializing")
        
      
        if result is None:
            print("Empty result received in PipeCompilerInterface.")

        return result
    
    def check_pipe_exists(PIPE_PATH):
        if os.path.exists(PIPE_PATH):
            if stat.S_ISFIFO(os.stat(PIPE_PATH).st_mode):
                return True
            else:
                return False
        else:
            return False

 
    def init_pipes(self, pipe_name=None):
        print("Initializing pipes")
        if pipe_name is not None:
            self.pipe_name = pipe_name
       
        
        self.to_compiler = self.pipe_name + ".in"
        self.from_compiler = self.pipe_name + ".out"
        if os.path.exists(self.to_compiler):
            os.remove(self.to_compiler)
        if os.path.exists(self.from_compiler):
            os.remove(self.from_compiler)
        print("Creating pipes: ", self.to_compiler, self.from_compiler)
        os.mkfifo(self.to_compiler, 0o666)
        os.mkfifo(self.from_compiler, 0o666)
        print("Pipes created")

    ## Resets the buffered reader/writers.
    def reset_pipes(self):
        print("Resetting pipes")
        
        self.tc = io.BufferedWriter(io.FileIO(self.to_compiler, "wb"))
        print("Opened writer")
        self.fc = io.BufferedReader(io.FileIO(self.from_compiler, "rb"))
        print("Pipes reset")
        
    def close_pipes(self):
        if self.fc is not None:
            self.tc.close()
            self.fc.close()
            self.tc = None
            self.fc = None

   
    def remove_pipes(self):
        os.remove(self.to_compiler)
        os.remove(self.from_compiler)
