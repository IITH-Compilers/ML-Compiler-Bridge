# ------------------------------------------------------------------------------
#
# Part of the MLCompilerBridge Project, under the Apache License v2.0 with LLVM
# Exceptions. See the LICENSE file for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# ------------------------------------------------------------------------------
##
## @file
## This file contains the abstract class for compiler interface. It specifies the
## methods for communication with compiler. It also initializes the correct
## SerDes object for serialization and deserialization of data.
##
# ------------------------------------------------------------------------------


from abc import ABC, abstractmethod
from .SerDes import SerDes

## This base class specifies methods for communication with compiler.
class BaseCompilerInterface(ABC):
    ## Initializes correct SerDes object
    def __init__(self, data_format=None):
        self.serdes_obj = SerDes(data_format)

    ## Places data for next request into a buffer after serialization.
    # @param Unserialized data for next query to compiler
    def populate_buffer(self, data):
        self.serdes_obj.serializeData(data)

    @abstractmethod
    ## Sends query to compiler and returns deserialized result.
    def evaluate(self):
        pass
