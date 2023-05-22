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

plt.rcParams['font.family'] = 'Source Han Code JP'

num_dev = 100
num_frame = 0
field_size = 60

fname = []
dev_x = []
dev_y = []
pos_x = []
pos_y = []

red = [0]
green = []

for i in range(num_dev):
    fname.append("tmp/dev_pos" + str(i) +".csv")
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

fig = plt.figure(figsize=(5,5))
ax = fig.add_subplot(111)
ax.set_aspect('equal')

def update(frame):
    ax.cla()
    ax.set_title("デバイス frame:" +str(frame), size=15)
    ax.set_xlabel("x", size=10)
    ax.set_ylabel("y", size=10)
    ax.set_xlim(0, field_size)
    ax.set_ylim(0, field_size)
    ax.plot(pos_x[frame], pos_y[frame], "o", c='k', ms=2)
    for id in red:
        ax.plot(pos_x[frame][id],pos_y[frame][id], "o", c='r', ms=5)
        c = patches.Circle(xy=(pos_x[frame][id], pos_y[frame][id]), radius=10, ec='k', fill=False)
        ax.add_patch(c)
    for id in green:
        ax.plot(pos_x[frame][id],pos_y[frame][id], "o", c='g', ms=5)
    

anim = FuncAnimation(fig, update, frames=range(num_frame), interval=200)


plt.show()

