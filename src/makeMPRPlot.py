'''
   file: makePositonPlot.py
   author: tom96da
   brief: シミュレーションでのデバイスの動きと接続をgifにする
   date: 2023-05-17
'''

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import matplotlib.patches as patches
import matplotlib.ticker as ticker
import pandas as pd


plt.rcParams['font.family'] = 'sans-serif'

num_dev = 100
num_frame = 0
field_size = 60

fname = []
dev_x = []
dev_y = []
pos_x = []
pos_y = []

target = [0]
first = [9, 62, 69, 97]
second = [32, 71]
third = [4, 44, 75, 53, 5, 21, 86]

for i in range(num_dev):
    fname.append("../tmp/dev_pos" + str(i) + ".csv")
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
    ax.set_title("MPR", size=18, fontweight='bold')
    ax.set_xlabel("x", size=18, fontweight='bold')
    ax.set_ylabel("y", size=18, fontweight='bold')
    ax.set_xlim(15, 55)
    ax.set_ylim(15, 55)
    ax.xaxis.set_major_locator(ticker.FixedLocator([15, 25, 35, 45, 55]))
    ax.yaxis.set_major_locator(ticker.FixedLocator([15, 25, 35, 45, 55]))

    ax.plot(pos_x[frame], pos_y[frame], "o", c='k', ms=2)
    for id in [4]:
        ax.plot([pos_x[frame][9], pos_x[frame][id]], [pos_y[frame][9],
                pos_y[frame][id]], c='lightgray', linestyle='dashed')
    for id in [75]:
        ax.plot([pos_x[frame][62], pos_x[frame][id]], [pos_y[frame][62],
                pos_y[frame][id]], c='lightgray', linestyle='dashed')
    for id in [44, 53]:
        ax.plot([pos_x[frame][69], pos_x[frame][id]], [pos_y[frame][69],
                pos_y[frame][id]], c='lightgray', linestyle='dashed')
    for id in [5, 21, 86]:
        ax.plot([pos_x[frame][97], pos_x[frame][id]], [pos_y[frame][97],
                pos_y[frame][id]], c='lightgray', linestyle='dashed')
    # for id in [5, 40, 44, 86]:
    #     ax.plot([pos_x[frame][97], pos_x[frame][id]], [pos_y[frame][97],
    #             pos_y[frame][id]], c='lightgray', linestyle='dashed')

    for id in third:
        ax.plot(pos_x[frame][id], pos_y[frame][id], "o", c='g', ms=3)

    for id in second:
        ax.plot([pos_x[frame][0], pos_x[frame][id]], [pos_y[frame][0],
                pos_y[frame][id]], c='gray', linestyle='dashed')
        ax.plot(pos_x[frame][id], pos_y[frame][id], "o", c='c', ms=4)

    for id in first:
        ax.plot([pos_x[frame][0], pos_x[frame][id]], [pos_y[frame][0],
                pos_y[frame][id]], c='gray', linestyle='dashed')
        ax.plot(pos_x[frame][id], pos_y[frame][id], "o", c='y', ms=5)

    for id in target:
        ax.plot(pos_x[frame][id], pos_y[frame][id], "o", c='r', ms=6)
        c = patches.Circle(
            xy=(pos_x[frame][id], pos_y[frame][id]), radius=10, ec='k', fill=False, linestyle='dashed')
        ax.add_patch(c)

    ax.plot(-1, -1, "o", c='y', ms=3, label='MPR')
    ax.plot(-1, -1, "o", c='c', ms=3, label='neighbors')
    ax.plot(-1, -1, "o", c='g', ms=3, label='2-hop neighbors')
    plt.legend(fontsize=12, prop={"weight": "bold"},
               loc="lower right", bbox_to_anchor=(1.0, 0.0))


anim = FuncAnimation(fig, update, frames=range(num_frame), interval=200)

anim.save("../tmp/fig/position.gif")
plt.show()
