from time import sleep
from os import system
import glob
from subprocess import STDOUT, check_output, TimeoutExpired

saidaDadosPath = "dados.txt"

configs = [(1, 1)]

executable = "knn"
nbExections = 3
maxTime = 9000

system("make")

for config in configs:
    for K in range(1, 101):
        print("Executando: {} {}".format(config, K))
        totalTime = 0
        flagExecutou = True
        message = "Done"
        for i in range(nbExections):
            print("Execução: {}".format(i))
            try:
                commandLine = "./" + executable + " " + str(K)
                print(commandLine)
                output = check_output(commandLine.split(), stderr=STDOUT, timeout=maxTime)
                output = output.decode("utf-8")
                totalTime += float(output)
            except TimeoutExpired:
                message = "TimeoutExpired"
                flagExecutou = False
            except:
                message = "ERRO"    
                flagExecutou = False
        print(message)
        if (flagExecutou):
            totalTime /= nbExections
            arq = open(saidaDadosPath, "a")
            arq.write("{} {} {} {}\n".format(K, config[0], config[1], totalTime))
            arq.close()
            print("{} {} {} {}\n".format(K, config[0], config[1], totalTime))
        else:
            arq = open(saidaDadosPath, "a")
            arq.write("{} {}\n".format(instance, message))
            arq.close()