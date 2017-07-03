# Executa codigos em C para os carros em multiplas janelas de terminal.
# Caso nao funcione na sua distribuicao, troque a chamada de terminal no loop
from subprocess import Popen, PIPE
import sys
import re

# Aceita alguns parametros para execucao do script.
def main():
    if len(sys.argv) < 3:
        sys.exit("ERROR: At least 2 parameters are needed. First is "
                 + "number of cars, second is server name")
    numberCars = int(sys.argv[1])

    fcars = open("carParameters", "r")
    carList = fcars.readlines()
    fcars.close()

    for i in range(numberCars):
        carParameters = re.findall(r"[-]?[\w]+", carList[i])
        print(carParameters)
        cmd = ["konsole", "--noclose", "-e", "./carClient", sys.argv[2], str(i)]
        for parameter in carParameters:
            cmd += [parameter]
        Popen(cmd)

if __name__ == "__main__":
    main()
