'''
   file: makePosGif.py
   author: tom96da
   brief: シミュレーションでのデバイスの動きをgifにする
   date: 2023-05-17
'''

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import matplotlib.patches as patches
import pandas as pd

plt.rcParams['font.family'] = 'sans-serif'

num_dev = 100
field_size = 60

fname = []
dev_x = []
dev_y = []
pos_x = []
pos_y = []

red = [3]
cyan = [
    [2, 9, 13, 19, 21],
    [6, 11, 25, 36, 88],
    [24, 29, 41, 42, 52, 67, 78, 81],
    [23, 43, 50, 51, 54, 66, 80, 86, 94, 97],
    [4, 5, 18, 20, 37, 53, 56, 57, 62, 73, 74, 83, 87, 92, 93, 98],
    [0, 15, 16, 38, 61, 63, 65, 72, 76, 77, 82],
    [8, 10, 14, 22, 28, 30, 35, 46, 55, 58, 60, 70, 71, 85, 89],
    [7, 17, 39, 48, 49, 64, 69, 75, 79],
    [27, 31, 33, 40, 44, 59, 68],
    [34, 45, 84, 90, 91, 95],
    [1, 47, 96, 99],
    [12, 26, 32]
]
yellow = []

num_step = len(cyan) + 1

for i in range(num_dev):
    fname.append("../tmp/dev_pos" + str(i) + ".csv")
    data = pd.read_csv(fname[i])

    dev_x.append(data['x'])
    dev_y.append(data['y'])

num_move = len(dev_x[0])

for i in range(num_move):
    frm_x = []
    frm_y = []
    for j in range(num_dev):
        frm_x.append(dev_x[j][i])
        frm_y.append(dev_y[j][i])
    pos_x.append(frm_x)
    pos_y.append(frm_y)

fig = plt.figure(figsize=(5, 6))
ax = fig.add_subplot(111)
ax.set_aspect('equal')
ax.tick_params(labelsize=12)


def update(frame):
    steps = frame
    move = 0
    ax.cla()
    ax.set_title("Step: " + str(steps), size=18, fontweight='bold')
    ax.set_xlabel("x", size=18, fontweight='bold')
    ax.set_ylabel("y", size=18, fontweight='bold')
    ax.set_xlim(0, field_size)
    ax.set_ylim(0, field_size)

    ax.plot(pos_x[move], pos_y[move], "o", c='k', ms=2)
    for id in red:
        ax.plot(pos_x[move][id], pos_y[move][id], "o", c='r', ms=5)
        # c = patches.Circle(
        #     xy=(pos_x[move][id], pos_y[move][id]), radius=10, ec='k', fill=False)
        # ax.add_patch(c)
    for step in range(steps):
        for id in cyan[step]:
            ax.plot(pos_x[move][id], pos_y[move][id], "o", c='c', ms=4)
    # for id in yellow:
    #     ax.plot(pos_x[move][id], pos_y[move][id], "o", c='g', ms=3)
    ax.plot(-1, -1, "o", c='r', ms=3, label='origin')
    ax.plot(-1, -1, "o", c='c', ms=3, label='reached data')
    plt.legend(fontsize=12, prop={"weight": "bold"},
               loc="lower right", bbox_to_anchor=(1.05, -0.22))


anim = FuncAnimation(fig, update, frames=range(num_step), interval=600)

# anim.save("anim.gif")
plt.show()
