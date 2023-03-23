#!/bin/python3
# THIS SHORT SCRIPT IS USED TO CLEAN UP THE SYSCALLS FOLDER AFTER COMPILING AND RUNNING THEM
import os

fold = os.listdir("../independent tests/syscalls")

for folder in fold:
    cont = os.listdir("../independent tests/syscalls/" + folder)
    for file in cont:
        path = "../independent tests/syscalls/" + folder + "/" + file
        if(".c" in file or ".h" in file):
            continue
        try:
            os.remove(path)
        except:
            print(path)