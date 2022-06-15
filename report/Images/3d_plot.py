import matplotlib as mpl
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import matplotlib.pyplot as plt

mpl.rcParams['legend.fontsize'] = 10

fig = plt.figure()
ax = fig.gca(projection='3d')
theta = np.linspace(-4 * np.pi, 4 * np.pi, 100)
z = np.linspace(-2, 2, 100)
r = z**2 + 1
x = r * np.sin(theta)
y = r * np.cos(theta)


def get_last_of_each() :
    x = []
    y = []
    z = []

    # Get last of each
    with open("log.csv", "r") as file :
        last_line = ''
        for line in file.readlines() :
            if line[0] == "=" :
                x.append(float(last_line.split(",")[0]))
                y.append(float(last_line.split(",")[1]))
                z.append(float(last_line.split(",")[2]))

            last_line = line

    return x, y, z



def get_all_of_t(t) :
    x = []
    y = []
    z = []

    # Get last of each
    with open("log.csv", "r") as file :
        crossed = 0
        for line in file.readlines() :
            if line[0] != "=" and crossed == t:
                x.append(float(line.split(",")[0]))
                y.append(float(line.split(",")[1]))
                z.append(float(line.split(",")[2]))
            else :
                crossed += 1
                if crossed > t :
                    print(x[0])
                    return x, y, z

    return x, y, z


def get_all() :
    x = []
    y = []
    z = []
    with open("log.csv", "r") as file :
        for line in file.readlines() :
            if line[0] != "=" :
                x.append(float(line.split(",")[0]))
                y.append(float(line.split(",")[1]))
                z.append(float(line.split(",")[2]))
    return x, y, z


colors = ['red', 'blue', 'green', 'black', 'pink', 'orange', 'purple', 'yellow', 'brown', 'gray', 'green', 'black']

x = []
y = []
z = []
i = 0
with open("log.csv", "r") as file :
    for line in file.readlines() :
        if line[0] != "=" :
            x.append(float(line.split(",")[0]))
            y.append(float(line.split(",")[1]))
            z.append(float(line.split(",")[2]))
        else :
            ax.plot(x, y, z, label='oui', c=colors[i])
            i += 1
            x = []
            y = []
            z = []

plt.show()
