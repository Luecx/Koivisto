import sys
import matplotlib.pyplot as plt

class TurnData:
    def __init__(self, _time, _depth, _seldepth, _score, _nodes, _nps):
        self.time = _time
        self.depth = _depth
        self.seldepth = _seldepth
        self.score = _score
        self.nodes = _nodes
        self.nps = _nps


def read_file(file):
    turns = {}

    f = open(file, "r")


    last_comm = ""

    for line in f:
        if not line.startswith("<"):
            continue

        name = line[1:line.find(":")]
        comm = line[line.find(":")+1:].strip()

        if comm.startswith("bestmove"):
            split = last_comm.split(" ")

            _time = int(split[split.index("time")+1])
            _depth = int(split[split.index("depth")+1])
            _seldepth = int(split[split.index("seldepth")+1])
            _score = int(split[split.index("score")+2])
            _nodes = int(split[split.index("nodes")+1])
            _nps = int(split[split.index("nps")+1])

            t = TurnData(_time, _depth, _seldepth, _score, _nodes, _nps)

            if not name in turns:
                turns[name] = []

            turns[name].append(t)

        else:
            last_comm = comm

    return turns

def extract(dat, name, type):
    if name not in dat:
        return None

    ar = []

    for v in dat[name]:
        ar.append(v.__getattribute__(type))

    return ar

def plotData(dat, type):
    keys = list(dat.keys())
    p1 = keys[0]
    p2 = keys[1]

    d1 = extract(dat, p1, type)
    d2 = extract(dat, p2, type)

    x1 = range(0, len(d1), 1)
    x2 = range(0, len(d2), 1)

    plt.ylabel(type)
    plt.xlabel('Move')

    # plt.ylim(top=30)    #ymax is your value
    # plt.ylim(bottom=0)  #ymin is your value

    plt.plot(x1, d1)
    plt.plot(x2, d2)

    plt.show()

if __name__ == "__main__":


    dat = read_file(sys.argv[1])
    plotData(dat,"time")
