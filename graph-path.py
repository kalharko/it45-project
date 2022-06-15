#!/bin/env python

import csv
import sys
import math
import statistics

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

coordinates = []
with open(sys.argv[1] + "/Coordinates.csv") as csv_file:
    reader = csv.reader(csv_file)
    for row in reader:
        coordinates.append((float(row[0]), float(row[1])))

days = []
with open(sys.argv[1] + "/Missions.csv") as csv_file:
    reader = csv.reader(csv_file)
    for row in reader:
        days.append(int(row[1]))

assignments = []
with open(sys.argv[2]) as csv_file:
    reader = csv.reader(csv_file)
    for row in reader:
        assignments = [int(x) for x in row]

def draw(a, b, agent, dotted = False):
    color = [
        "black",
        "magenta",
        "cyan",
        "olive",
        "violet",
        "blue",
        "brown",
        "red",
        "orange",
        "pink",
        "purple"
    ][agent];
    print(
        "  \\draw[draw="
        + color
        + (",densely dotted" if dotted else "")
        + "] (" + str(a + 1) + ") -- (" + str(b + 1) + ");"
    )

    if a == -1:
        a = (0, 0)
    else:
        a = coordinates[a]

    if b == -1:
        b = (0, 0)
    else:
        b = coordinates[b]

    return math.sqrt((a[0] - b[0]) ** 2.0 + (a[1] - b[1]) ** 2.0)

def points():
    max_x = 0
    min_x = 0
    max_y = 0
    min_y = 0

    for coords in coordinates:
        max_x = max(coords[0], max_x)
        min_x = min(coords[0], min_x)
        max_y = max(coords[1], max_y)
        min_y = min(coords[1], min_y)

    dist = max(max_x - min_x, max_y - min_y)
    mult = 10.0 / dist

    print("  \\node[rectangle,fill,inner sep=1pt] at (0, 0) (0) {};")
    for (index, coords) in enumerate(coordinates):
        x = coords[0] * mult
        y = coords[1] * mult
        print("    \\node[circle,fill,inner sep=1pt] at (" + str(x) + ", " + str(y) + ") (" + str(index + 1) + ") {};")

if len(sys.argv) == 4:
    agent = int(sys.argv[3])

    print("\\center")
    print("\\begin{tikzpicture}")

    points()

    missions = [[], [], [], [], [], [], []]
    for (index, mission) in enumerate(assignments):
        if mission == agent:
            missions[days[index]].append(index)

    for day in missions:
        if len(day) > 0:
            draw(-1, day[0], agent, True)
            for i in range(1, len(day)):
                draw(day[i - 1], day[i], agent)
            draw(day[len(day) - 1], -1, agent, True)

    print("\\end{tikzpicture}")
else:
    print("\\center")
    print("\\begin{tikzpicture}")

    points()

    dists = []
    for agent in range(max(assignments) + 1):
        dist_sum = 0
        missions = [[], [], [], [], [], [], []]
        for (index, mission) in enumerate(assignments):
            if mission == agent:
                missions[days[index]].append(index)

        for day in missions:
            if len(day) > 0:
                dist_sum += draw(-1, day[0], agent, True)
                for i in range(1, len(day)):
                    dist_sum += draw(day[i - 1], day[i], agent)
                dist_sum += draw(day[len(day) - 1], -1, agent, True)
        dists.append(dist_sum)

    print("\\end{tikzpicture}")
    eprint(str(sum(dists) / 1000.0), "±", str(statistics.stdev(dists) / 1000.0))

# print(coordinates)
# print(assignments)
