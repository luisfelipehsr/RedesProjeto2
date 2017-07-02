# Executa codigos em C para os carros em multiplas janelas de terminal.
# Caso nao funcione na sua distribuicao, troque a chamada de terminal no loop
from subprocess import Popen, PIPE
import sys


# Aceita alguns parametros para execucao do script.
def main():
    if len(sys.argv) < 2:
        sys.exit("ERROR: At least 1 parameter is needed")

    numberCars = int(sys.argv[1])

    for i in range(numberCars):
        Popen(["konsole", "--noclose", "-e",
               "./testeWait"])

if __name__ == "__main__":
    main()
