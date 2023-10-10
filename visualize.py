# python visualize.py SET4/100-20-3-2 0
import sys
import matplotlib.pyplot as plt

# === ARGUMENT HANDLING ========================================================
fileName = sys.argv[1]
save = int(sys.argv[2])

# === FILE READING =============================================================
fin = open("instances/OPHS_instances_February 2013/" + fileName + ".ophs", 'r', 1)
instancesF = fin.read()
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
            print(f'x: {float(tempX)} y: {float(tempY)}')
        else:
            pass
    elif not line:
        enable = True

maxScore = 0
for i in score:
    if i > maxScore:
        maxScore = i

# === PLOT RENDERING ===========================================================
plt.figure(figsize=(8, 8))
ax = plt.gca()
ax.set_facecolor((0.1, 0.1, 0.1))
for i in range(0, len(x)):
    if score[i] == 0.0:
        if i == 0:
            plt.scatter(x[i], y[i], s = 100, c = "blue") # hotel inicio
        elif i == 1:
            plt.scatter(x[i], y[i], s = 100, c = "white") # hotel final
        else:
            plt.scatter(x[i], y[i], s = 100, c = "red") # outros hotéis
    else:
        col = score[i]/maxScore
        plt.scatter(x[i], y[i], s = 25 + 25 * col, c = ([0.5*(1 + col), col, col]))
if save:
    plt.savefig("out/" + fileName + ".png", bbox_inches='tight')
else:
    plt.show()
