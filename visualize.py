"""
Para executar:
    Criar pasta SET4 dentro da pasta out
    criar um arquivo com o nome 100-20-3-2.ophsout
    colocar o seguinte conteudo
        0 54 27 11 18 71 13 4 
        4 14 84 26 1
    python visualize.py SET4/100-20-3-2 0
"""
# python visualize.py SET4/100-20-3-2 0

import sys
import matplotlib.pyplot as plt

# === ARGUMENT HANDLING ========================================================
fileName = sys.argv[1]
save = int(sys.argv[2])
alg = sys.argv[3]

# === FILE READING =============================================================
fin = open("instances/OPHS_instances_February 2013/" + fileName + ".ophs", 'r', 1)
instancesF = fin.read()
fin.close()

fin = open("out/" + fileName + ".ophsout"+alg, 'r', 1)
tourF = fin.read()
fin.close()

# === VERTICES FILE READING ====================================================
x = []
y = []
score = []

enable = False
for line in instancesF.splitlines():
    if enable:
        if line[1] != '-':
            tempX, tempY, tempScore = line.split()
            x.append(float(tempX))
            y.append(float(tempY))
            score.append(float(tempScore))
        else:
            pass
    elif not line:
        enable = True

maxScore = 0
for i in score:
    if i > maxScore:
        maxScore = i
# === TOUR FILE READING ========================================================
tour = []

for line in tourF.splitlines():
    tour.append([])
    for i in line.split():
        tour[-1].append(int(i))

tourX = []
tourY = []
totalScore = 0
for trip in tour:
    tourX.append([])
    tourY.append([])
    for node in trip:
        tourX[-1].append(x[node])
        tourY[-1].append(y[node])
        totalScore += score[node]

# === PLOT RENDERING ===========================================================
plt.figure(figsize=(8, 8))
ax = plt.gca()
plt.title(fileName + " - Score: "+str(totalScore))
# ax.set_facecolor((0.1, 0.1, 0.1))
extra_hotels = 0
for i in range(0, len(x)):
    if score[i] == 0.0:
        if i == 0:
            plt.scatter(x[i], y[i], s = 500, c = "blue", label="Hotel Início", marker="*") # hotel inicio
        elif i == 1:
            plt.scatter(x[i], y[i], s = 500, c = "black", label="Hotel Final", marker="x") # hotel final
        else:
            extra_hotels+=1
            plt.scatter(x[i], y[i], s = 100, c = "yellow") # outros hotéis
    else:
        col = score[i]/maxScore
        color = [0.5 * (1 + col), col, col]
        plt.scatter(x[i], y[i], s = 40 + 25 * col, color=color)
for i in range(0, len(tourX)):
    plt.plot(tourX[i], tourY[i], c = ([0.0 + (i / len(tourX)), 0.0, 1.0 - (i / len(tourX))]))

plt.scatter([], [], s=100, c="yellow", label="Hotéis Extras: "+str(extra_hotels))  # Para que o label dos outros hoteis so apareça uma vez
plt.legend(markerscale=0.5)
if save:
    plt.savefig("out/" + fileName + "_"+alg+".svg", bbox_inches='tight')
else:
    plt.show()
