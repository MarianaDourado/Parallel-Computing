import matplotlib.pyplot as plt
import itertools

OUT = "graphs/"

def read(path):
    values = {}
    file = open(path, 'r')
    lines = file.readlines()
    file.close()
    
    for line in lines:
        k, nbThreads, nbBlocks, time = [int(float(x)) for x in line.split()]
        if ((nbThreads, nbBlocks) not in values):
            values[(nbThreads, nbBlocks)] = []
        values[(nbThreads, nbBlocks)].append((k, time))
    
    return values

def preprocessa(values, configs = ([(16, 16), (32, 32), (64, 64), (128, 128)])):
    maxK = 50
    
    newValues = {}
    for config in configs:
        newValues[config] = []
    
    for config, times in values.items():
        if (config not in configs):
            continue
        for k, time in times:
            if (k > maxK):
                continue
            newValues[config].append((k, time))
    return newValues

def plot(values, title, eixoX = 'K-size', eixoY = 'Time (ms)'):
    fig, ax = plt.figure(), plt.gca()

    colors = itertools.cycle(plt.cm.tab10.colors)

    for entry in values:
        for key, coordinates in entry.items():
            x = [coord[0] for coord in coordinates]
            y = [coord[1] for coord in coordinates]
            
            ax.plot(x, y, linestyle='-', color=next(colors), label=str(key))

    ax.set_title(title)
    ax.set_xlabel(eixoX)
    ax.set_ylabel(eixoY)
    ax.grid(True)
    ax.legend()
    plt.tight_layout(pad=1)
    plt.savefig(OUT + title + ".png", bbox_inches='tight', pad_inches=0.1)
    plt.show()

def plotSpeedup(seqValue, values, title, eixoX = 'K-size', eixoY = 'Speedup'):
    def get(times, target):
        for k, time in times:
            if (k == target):
                return time
        raise Exception("Do no found value {}.".format(target))
        return 0
    
    fig, ax = plt.figure(), plt.gca()

    colors = itertools.cycle(plt.cm.tab10.colors)

    for entry in values:
        for key, coordinates in entry.items():
            x = [coord[0] for coord in coordinates]
            y = [get(seqValue[(1, 1)], coord[0])/coord[1] for coord in coordinates]
            
            ax.plot(x, y, linestyle='-', color=next(colors), label=str(key))

    ax.set_title(title)
    ax.set_xlabel(eixoX)
    ax.set_ylabel(eixoY)
    ax.grid(True)
    ax.legend()
    plt.tight_layout(pad=1)
    plt.savefig(OUT + title + ".png", bbox_inches='tight', pad_inches=0.1)
    plt.show()

valsSeq = preprocessa(read("dadosSeq.txt"), configs=([(1, 1)]))
valsSeqAll = preprocessa(read("dadosSeqAll.txt"), configs=([(1, 1)]))
valsParalelo = preprocessa(read("dadosParalelo.txt"))
valsAllParalelo = preprocessa(read("dadosAllParalelo.txt"))

plot(([{'Sequencial': valsSeq[(1, 1)]}]), 'Tempo sequencial de um teste')
plot(([{'Sequencial': valsSeqAll[(1, 1)]}]), 'Tempo sequencial de todos os testes')
plot(([valsParalelo]), 'Tempo paralelo de um teste')
plot(([valsAllParalelo]), 'Tempo paralelo de todos os testes')
plot(([{'Sequencial': valsSeq[(1, 1)]}, valsParalelo]), 'Tempo de um teste')
plot(([{'Sequencial': valsSeqAll[(1, 1)]}, valsAllParalelo]), 'Tempo de todos os teste')

plotSpeedup(valsSeq, ([valsParalelo]), 'Speedup de um teste')
plotSpeedup(valsSeqAll, ([valsAllParalelo]), 'Speedup de todos os testes')