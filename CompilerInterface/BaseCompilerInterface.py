from abc import ABC, abstractmethod
from SerDes import SerDes
import os
import io

## This base class specifies methods for communication with compiler.
class BaseCompilerInterface(ABC):
    ## Initializes correct SerDes object
    def __init__(self, data_format=None):
        self.serdes_obj = SerDes(data_format) 

    ## Sets content of next request into a buffer.
    def populate_buffer(self, data):
        self.serdes_obj.serializeData(data)

    @abstractmethod
    ## Sends query to compiler and returns result.
    def evaluate(self):
        pass


