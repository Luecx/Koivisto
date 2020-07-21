import sys
import matplotlib.pyplot as plt
import numpy as np
from sklearn.linear_model import LinearRegression


class TurnData:
    def __init__(self, _time, _depth, _seldepth, _score, _nodes, _nps):
        self.time = _time
        self.depth = _depth
        self.seldepth = _seldepth
        self.score = _score
        self.nodes = _nodes
        self.nps = _nps


def estimate_coef(x, y):
    # number of observations/points
    n = np.size(x)

    # mean of x and y vector
    m_x, m_y = np.mean(x), np.mean(y)

    # calculating cross-deviation and deviation about x
    SS_xy = np.sum(y*x) - n*m_y*m_x
    SS_xx = np.sum(x*x) - n*m_x*m_x

    # calculating regression coefficients
    b_1 = SS_xy / SS_xx
    b_0 = m_y - b_1*m_x

    return(b_0, b_1)


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

    x1 = np.arange(0, len(d1), 1).reshape((-1,1))
    x2 = np.arange(0, len(d2), 1).reshape((-1,1))

    plt.ylabel(type)
    plt.xlabel('Move')


    model1 = LinearRegression()
    model2 = LinearRegression()

    model1.fit(x1,d1);
    model2.fit(x2,d2);

    y1_pred = model1.intercept_ + model1.coef_*x1
    y2_pred = model2.intercept_ + model2.coef_*x1

    plt.plot(x1, y1_pred, color = "r", linestyle= "--")
    plt.plot(x2, y2_pred, color = "g", linestyle= "--")



    plt.plot(x1, d1, label=p1, color = "r")
    plt.plot(x2, d2, label=p2, color = "g")

    plt.legend()

    plt.show()

if __name__ == "__main__":


    dat = read_file(sys.argv[1])
    plotData(dat,"nps")
