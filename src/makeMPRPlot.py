'''
   file: makePositonPlot.py
   author: tom96da
   brief: シミュレーションでのデバイスの動きと接続をgifにする
   date: 2023-05-17
'''

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import matplotlib.patches as patches
import matplotlib.ticker as ticker
import pandas as pd

plt.rcParams['font.family'] = 'Noto Sans CJK JP'

num_dev = 100
num_frame = 0
field_size = 60
u = 2

fname = []
dev_x = []
dev_y = []
pos_x = []
pos_y = []

target = 7
neighbors = [51]
MPRs = [21, 33, 45, 75, 90]
tow_hop = [[4, 41, 52, 69],
           [74, 83],
           [18, 19],
           [28, 34, 53, 58],
           [80],
           ]

# target = 7
# neighbors = []
# MPRs = [4, 18, 21, 33, 74, 80]
# tow_hop = [[11, 42],
#            [19, 34, 87],
#            [41, 44, 52, 81],
#            [45, 51],
#            [9, 28, 63, 83, 90],
#            [23, 47, 58],
#            ]

for i in range(num_dev):
    fname.append("../tmp/position/device" + str(i) + ".csv")
    data = pd.read_csv(fname[i])

    dev_x.append(data['x'])
    dev_y.append(data['y'])

num_frame = len(dev_x[0])

for i in range(num_frame):
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
    ax.cla()
    ax.set_title("MPR", size=20, fontweight='bold')
    ax.set_xlabel("$x$", size=20)
    ax.set_ylabel("$y$", size=20)
    # ax.set_xlim(0, 60)
    # ax.set_ylim(0, 60)
    ax.set_xlim(15 - u, 55 + u)
    ax.set_ylim(15 - u, 55 + u)
    # ax.xaxis.set_major_locator(ticker.FixedLocator(np.arange(0, 61, 10)))
    ax.xaxis.set_major_locator(ticker.FixedLocator(np.arange(5, 56, 10)))
    # ax.yaxis.set_major_locator(ticker.FixedLocator(np.arange(0, 61, 10)))
    ax.yaxis.set_major_locator(ticker.FixedLocator(np.arange(5, 56, 10)))
    plt.rcParams['xtick.direction'] = 'in'
    plt.rcParams['ytick.direction'] = 'in'

    ax.tick_params(axis='both', which='major', labelsize=14)

    ax.plot(pos_x[frame], pos_y[frame], "o", c='k', ms=2)

    for i in range(len(MPRs)):
        for id in tow_hop[i]:
            ax.plot([pos_x[frame][MPRs[i]], pos_x[frame][id]], [pos_y[frame][MPRs[i]],
                    pos_y[frame][id]], c='darkgray', linewidth=2, linestyle='dashed')

    for ids in tow_hop:
        for id in ids:
            ax.plot(pos_x[frame][id], pos_y[frame][id], "o", c='g', ms=4)

    for id in neighbors:
        ax.plot([pos_x[frame][target], pos_x[frame][id]], [pos_y[frame][target],
                pos_y[frame][id]], c='dimgray', linewidth=2, linestyle='dashed')
        ax.plot(pos_x[frame][id], pos_y[frame][id], "o", c='c', ms=6)

    for id in MPRs:
        ax.plot([pos_x[frame][target], pos_x[frame][id]], [pos_y[frame][target],
                pos_y[frame][id]], c='dimgray', linewidth=2, linestyle='dashed')
        ax.plot(pos_x[frame][id], pos_y[frame][id], "o", c='y', ms=6)

    ax.plot(pos_x[frame][target], pos_y[frame][target], "o", c='r', ms=8)
    c = patches.Circle(
        xy=(pos_x[frame][target], pos_y[frame][target]), radius=10, linewidth=1.5, ec='k', fill=False, linestyle='dashed')
    ax.add_patch(c)

    ax.plot(-1, -1, "o", c='y', ms=5, label='MPR')
    ax.plot(-1, -1, "o", c='c', ms=5, label='neighbors')
    ax.plot(-1, -1, "o", c='g', ms=4, label='2-hop neighbors')
    # plt.legend(prop={"weight": "bold", "size": "large"}, loc="upper right")
    plt.legend(prop={"weight": "bold", "size": "large"}, loc="lower right")


anim = FuncAnimation(fig, update, frames=range(num_frame), interval=200)

anim.save("../tmp/fig/position.gif")
plt.show()
