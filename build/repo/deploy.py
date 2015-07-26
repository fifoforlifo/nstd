import os
import pynja
from .root_paths import *
from .cpp import *

class DeployVariant(pynja.Variant):
    def __init__(self, string):
        super().__init__(string, self.get_field_defs())

    def get_field_defs(self):
        fieldDefs = [
            "toolchain",    CppVariant.fieldDefs[1*2 + 1],        
            "arch",         CppVariant.fieldDefs[2*2 + 1],        
            "config",       CppVariant.fieldDefs[3*2 + 1],
        ]
        return fieldDefs

class DeployProject(pynja.DeployProject):
    def __init__(self, projectMan, variant):
        super().__init__(projectMan, variant)
        if not isinstance(variant, DeployVariant):
            raise Exception("variant must be instanceof(DeployVariant)")
