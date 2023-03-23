#!/bin/python3

import os
import subprocess

logs = ""

# STUB COMMANDS, TO BE REPLACED WHEN THE EFILOADER WORKS

runCmd = "/root/a.out > /root/log.txt"
rootPath = "/root/syscalls"

pass_list = [0] * 361
fail_list = [0] * 361
brok_list = [0] * 361
#run a test
def runTest(name, path):
    try:
        # compile the test
        global logs
        log = False
        subprocess.run("cd " + path + "&& gcc " + name + "> /root/comp.txt", shell=True)
        with open("/root/comp.txt", "r") as file:
            cont = file.read()
            logs += name + "\n" + cont + "\n"
            log = True
            file.close()
        content = os.listdir(path)
        if("a.out" not in content):
            return 0
        # move the resulting binary in the elfloader shared folder
        subprocess.run("mv " + path + "/a.out /root/", shell=True)
        # run the image, outputting the results in a log file
        subprocess.run(runCmd, shell=True)
        cont = ""
        with open("/root/log.txt", "r") as file:
            cont = file.read()
            file.close()
        if("test succeeded" in cont):
            return 1
        else:
            if(log is False):
                logs += name
            logs += "\n" + cont + "DIVIDER"
            return 0
    except:
        return -1

# looks through the test files, compiling the lists
syscalls = os.listdir(rootPath)
for i in range(len(syscalls)):
    fpath = rootPath + "/" + syscalls[i]
    fcont = os.listdir(fpath)
    if(len(fcont) != 1 and "incl.h" not in fcont):
        brok_list[i] += len(fcont)
        continue
    for file in fcont:
        if(".h" in file):
            continue
        res = runTest(file, fpath)
        if(res == 0):
            fail_list[i] += 1
        elif(res == 1):
            pass_list[i] += 1
        else:
            brok_list[i] += 1

# creates and prints the output
out = ""
for i in range(len(syscalls)):
    out += str(pass_list[i]) + " "
out += "\n"
for i in range(len(syscalls)):
    out += str(fail_list[i]) + " "
out += "\n"
for i in range(len(syscalls)):
    out += str(brok_list[i]) + " "

out += "DIVIDER" + logs
f = open("/root/out.txt", "w")
f.write(out)