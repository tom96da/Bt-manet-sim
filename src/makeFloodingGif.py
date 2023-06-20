'''
   file: makeFloodingGif.py
   author: tom96da
   brief: シミュレーションでのフラッディングの様子をgifにする
   date: 2023-05-17
'''

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import pandas as pd

plt.rcParams['font.family'] = 'sans-serif'

num_dev = 100
field_size = 60

fname = []
dev_x = []
dev_y = []
pos_x = []
pos_y = []

source = [45]
received = [
    [72, 82, 86, 87, 90, 99],
    [5, 8, 12, 14, 21, 22, 23, 29, 46, 58, 61, 62, 65, 67, 71, 88, 94, 97],
    [0, 11, 13, 15, 17, 20, 31, 35, 36, 37, 38, 41, 47,
        49, 50, 55, 56, 63, 74, 79, 81, 83, 92, 93],
    [2, 3, 4, 9, 16, 24, 27, 39, 40, 51, 54, 66, 70, 75, 78, 91],
    [10, 18, 19, 33, 34, 64, 73, 76, 77, 84, 85, 95],
    [1, 6, 7, 25, 42, 44, 48, 53, 60, 80],
    [28, 30, 43, 52, 57, 59, 68, 89, 96, 98],
    [26, 32, 69],
]

num_step = len(received) + 1

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
    for id in source:
        ax.plot(pos_x[move][id], pos_y[move][id], "o", c='r', ms=5)

    for step in range(steps):
        for id in received[step]:
            ax.plot(pos_x[move][id], pos_y[move][id], "o", c='c', ms=4)

    ax.plot(-1, -1, "o", c='r', ms=3, label='origin')
    ax.plot(-1, -1, "o", c='c', ms=3, label='reached data')
    plt.legend(fontsize=12, prop={"weight": "bold"},
               loc="lower right", bbox_to_anchor=(1.05, -0.22))


anim = FuncAnimation(fig, update, frames=range(num_step), interval=600)

anim.save("../tmp/fig/flooding.gif")
plt.show()
