#!/usr/bin/env python
# -*- coding: utf-8 -*-
from PLCControler import *
import os

print paths.AbsParentDir(__file__)

def _getIECgeneratedcodePath(dir):
    return os.path.join(dir,"generated_plc.st")

if __name__ == '__main__':
    controller = PLCControler()
    ProjectDir = paths.AbsDir(__file__)
    print ProjectDir
    filePath = os.path.join(ProjectDir,"plc.xml")
    print filePath
    error = controller.OpenXMLFile(filePath)
    if error is not None:
        print("filePath is error\n")
    if controller.Project is None:
        print("Project is none\n")
    print controller.Project

    stcodePath = _getIECgeneratedcodePath(ProjectDir)
    print stcodePath

    program,errors,warnings = controller.GenerateProgram(_getIECgeneratedcodePath(ProjectDir))
    if len(warnings) > 0 or len(errors) > 0:
        print("error\n")
