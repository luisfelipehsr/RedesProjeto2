import re
import sys
import os

media_total_teste = []
for i in range(8):
    media_parcial_teste = []
    total_parcial = 0
    for j in range(3):
        media = []
        total = 0
        for k in range(10):
            local_dir = os.getcwd()
            #/home/jtstark/MC833/Projeto2/RedesProjeto2/TesteJoao/00jTest/1/arq0.out
            #/home/jtstark/MC833/Projeto2/RedesProjeto2/TesteJoao/00jTest/1/arq0.out
            #/home/jtstark/MC833/Projeto2/RedesProjeto2/00jTest/1/arq0.out
            home = local_dir + "/TesteJoao/0" + str(i) + "jTest/" + str(j+1) + "/arq" + str(k) + ".out"
            file = open(home, "r")
            lines = file.readlines()
            file.close()
            l = 0
            total = 0
            for line in lines:
                if re.search(r"Maximum delay", line) is not None:
                    valueList = re.findall(r"[0-9]+", line)
                    value = valueList[0]
                    total += int(value)
                    l += 1
            try:
                media.append(int(total / l))
                #print(str(i) + ' ' + str(j) + ' ' + str(k) + " = " + str(int(total / l)))
            except:
                #print("div zero")
                pass

        for k in range(len(media)):
            total += media[k]

        print(total)
        print(len(media))
        media_parcial_teste.append(int(total/len(media)))
        print(media_parcial_teste)

    for j in range(len(media_parcial_teste)):
        total_parcial += media_parcial_teste[j]

    media_total_teste.append(total_parcial/len(media_parcial_teste))

for i in range(8):
    print("Media " + str(i) + ": " + str(int(media_total_teste[i])))
