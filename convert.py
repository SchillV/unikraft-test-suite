#!/bin/python3

import os
import pathlib

def getFiles(path):
    content = os.listdir(path)
    files = []
    for file in content:
        if ".c" in file:
            with open(path + "/" + file) as f:
                try:
                    files.append([f.read(), file])
                except:
                    print(file)
                f.close()
    return files

def isMain(els):
    for el in els:
        if("main" in el):
            return True
    return False

def verfunc(el):
    ret = el.split("{")[0] + "{\n\tint ret;" 
    lines = el.split("{")[1].split("\n")
    for line in lines:
        if("SAFE_WRITE_ALL" in line):
                aux = line.split("SAFE_WRITE_ALL")
                line = aux[0] + "1" + aux[1]
        if("SAFE_" in line):
            aux = line.split("SAFE_")
            aux1 = aux[1].split("(")
            line = aux[0] + aux1[0].lower() + "(" + aux1[1]
            if(len(aux1) > 2):
                for i in range(1, len(aux1)):
                    line += aux1[i]
        if("TEST(" in line):
            line = "ret = " + line.split("TEST(")[1][:-2] + ";"
        if("TST_RET" in line):
            aux = line.split("TST_RET")
            line = aux[0] + "ret" + aux[1]
        if("tst_res" in line):
            if("TFAIL" in line):
                print("")
        ret += line + "\n"
    return ret
    


def convertFile(file):
    els = file.split("static ")
    ret = "#include \"incl.h\"\n"
    for el in els:
        if("struct tst_test test" in el):
            continue
        lines = el.split("\n")
        if("verify" in lines[0]):
            ret += verfunc(el) + "\n"
            continue
        for line in lines:
            if("//" in line or "/*" in line or "#include" in line or line == ""):
                continue
            if(len(line) > 1 and line[1] == "*"):
                continue
            if("SAFE_WRITE_ALL" in line):
                aux = line.split("SAFE_WRITE_ALL")
                line = aux[0] + "1" + aux[1]
            if("SAFE_" in line):
                aux = line.split("SAFE_")
                aux1 = aux[1].split("(")
                line = aux[0] + aux1[0].lower() + "(" + aux1[1]
                if(len(aux1) > 2):
                    for i in range(1, len(aux1)):
                        line += aux1[i]
            if("file_printf" in line):
                aux = line.split("file_printf")
                line = aux[0] + "fprintf" + aux[1]
            if("TEST(" in line):
                line = line.split("TEST(")[1][:-2] + ";"
            ret += line + "\n"
        ret += "\n"
    if(isMain(els) is False):
        return ret + "void main(){\n\tsetup();\n\tcleanup();\n}\n"
    else:
        return ret

def convertFiles(path):
    files = getFiles(path)
    outpath = "./independent tests/syscalls/" + path.split("/")[2]
    if(pathlib.Path(outpath).exists() is False):
        os.makedirs(outpath)
    for file in files:
        newfile = convertFile(file[0])
        with open (outpath + "/" + file[1], "w") as f:
            f.write(newfile)
            f.close()


inpath = "./original tests"
folders = os.listdir(inpath)
for folder in folders:
    convertFiles(inpath + "/" + folder)
