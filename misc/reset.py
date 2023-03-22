#!/bin/python3
import os

fold = os.listdir("./syscalls")

for folder in fold:
    cont = os.listdir("./syscalls/" + folder)
    for file in cont:
        path = "./syscalls/" + folder + "/" + file
        if(".c" in file or ".h" in file):
            continue
        try:
            os.remove(path)
        except:
            print(path)