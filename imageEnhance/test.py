# -*- coding: utf-8 -*-
import os
INPUT = "./data/"
OUTPUT = "./out/"

for aFile in os.listdir(INPUT):
    inputPath = os.path.join(INPUT, aFile)
    outputPath = os.path.join(OUTPUT, aFile)
    command = "bin/test " + inputPath + " " + outputPath
    os.system(command)
