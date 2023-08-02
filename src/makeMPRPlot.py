'''
   file: makePositonPlot.py
   author: tom96da
   brief: シミュレーションでのデバイスの動きと接続をgifにする
   date: 2023-05-17
'''

import matplotlib.pyplot as plt
import japanize_matplotlib
from matplotlib.animation import FuncAnimation
import matplotlib.patches as patches
import matplotlib.ticker as ticker
import pandas as pd

plt.rcParams['font.family'] = 'Noto Sans CJK JP'

num_dev = 200
num_frame = 0
field_size = 60

fname = []
dev_x = []
dev_y = []
pos_x = []
pos_y = []

target = 102
neighbors = []
MPRs = [61, 76, 88, 112, 125, 135]
tow_hop = [[57, 130, 155, 198],
           [89, 93],
           [108, 194],
           [116],
           [13, 22, 124, 143],
           [65, 78, 159, 172],
           ]

# target = 102
# neighbors = [76]
# MPRs = [26, 45, 61, 88, 89]
# tow_hop = [[83, 108, 142, 143],
#            [79, 103, 188, 199],
#            [13, 28, 65, 113, 198],
#            [78, 90, 166],
#            [93, 132, 135, 172],
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
    ax.set_xlim(15, 55)
    ax.set_ylim(10, 50)
    ax.xaxis.set_major_locator(ticker.FixedLocator([15, 25, 35, 45, 55]))
    ax.yaxis.set_major_locator(ticker.FixedLocator([10, 20, 30, 40, 50]))
    plt.rcParams['xtick.direction'] = 'in'
    plt.rcParams['ytick.direction'] = 'in'

    ax.tick_params(axis='both', which='major', labelsize=14)

    ax.plot(pos_x[frame], pos_y[frame], "o", c='k', ms=2)

    for i in range(len(MPRs)):
        for id in tow_hop[i]:
            ax.plot([pos_x[frame][MPRs[i]], pos_x[frame][id]], [pos_y[frame][MPRs[i]],
                    pos_y[frame][id]], c='darkgray', linestyle='dashed')

    for ids in tow_hop:
        for id in ids:
            ax.plot(pos_x[frame][id], pos_y[frame][id], "o", c='g', ms=4)

    for id in neighbors:
        ax.plot([pos_x[frame][target], pos_x[frame][id]], [pos_y[frame][target],
                pos_y[frame][id]], c='dimgray', linestyle='dashed')
        ax.plot(pos_x[frame][id], pos_y[frame][id], "o", c='c', ms=6)

    for id in MPRs:
        ax.plot([pos_x[frame][target], pos_x[frame][id]], [pos_y[frame][target],
                pos_y[frame][id]], c='dimgray', linestyle='dashed')
        ax.plot(pos_x[frame][id], pos_y[frame][id], "o", c='y', ms=6)

    ax.plot(pos_x[frame][target], pos_y[frame][target], "o", c='r', ms=8)
    c = patches.Circle(
        xy=(pos_x[frame][target], pos_y[frame][target]), radius=10, ec='k', fill=False, linestyle='dashed')
    ax.add_patch(c)

    ax.plot(-1, -1, "o", c='y', ms=5, label='MPR')
    ax.plot(-1, -1, "o", c='c', ms=5, label='neighbors')
    ax.plot(-1, -1, "o", c='g', ms=4, label='2-hop neighbors')
    plt.legend(prop={"weight": "bold", "size": "large"}, loc="upper right")


anim = FuncAnimation(fig, update, frames=range(num_frame), interval=200)

anim.save("../tmp/fig/position.gif")
plt.show()
