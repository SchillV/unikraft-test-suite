pop = []
red = ""
sysc = ""
out = ""
with open("order.txt", "r") as file:
    red = file.read()
red = red.split("\n")
for i in range(len(red)):
    red[i]=red[i].split(",")[0][3:]
for i in range(len(red)):
    sysc += red[i]
with open("order1.txt", "r") as file:
    red = file.read().split("\n")
for line in red:
    if(line.split(":")[0] not in sysc):
        continue
    out += line + "\n"
print(out)