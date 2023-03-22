import os

count = []
names = []

apps = os.listdir("./syscalls")

for app in apps:
    count.append(len(os.listdir("./syscalls/" + app)))
    names.append(app)

for i in range(len(count)-1):
    for j in range(i+1, len(count)):
        if(count[i]>count[j]):
            aux = count[i]
            count[i] = count[j]
            count[j] = aux
            aux = names[i]
            names[i] = names[j]
            names[j] = aux

for i in range(len(count)):
   print(names[i] + ": " + str(count[i]))
print("total: " + str(sum(count)))