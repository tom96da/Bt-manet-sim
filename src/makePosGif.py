'''
   file: makePosGif.py
   author: tom96da
   brief: シミュレーションでのデバイスの動きをgifにする
   date: 2023-05-17
'''

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import pandas as pd

plt.rcParams['font.family'] = 'Source Han Code JP'

num_dev = 100

fname = []
dev_x = []
dev_y = []
pos_x = []
pos_y = []

for i in range(0,num_dev):
    fname.append("tmp/dev_pos" + str(i) +".csv")
    data = pd.read_csv(fname[i])

    dev_x.append(data['x'])
    dev_y.append(data['y'])

for i in range(0,len(dev_x)):
    frm_x = []
    frm_y = []
    for j in range(num_dev):
        frm_x.append(dev_x[j][i])
        frm_y.append(dev_y[j][i])
    pos_x.append(frm_x)
    pos_y.append(frm_y)

fig = plt.figure(figsize=(5,5))
ax = fig.add_subplot(111)

def update(frame):
    ax.cla()
    ax.set_title("デバイス frame:" +str(frame), size=15)
    ax.set_xlabel("x", size=10)
    ax.set_ylabel("y", size=10)
    ax.set_xlim(0, 100)
    ax.set_ylim(0, 100)
    ax.plot(pos_x[frame], pos_y[frame], "o", c='k', ms=2)

anim = FuncAnimation(fig, update, frames=range(100), interval=500)


plt.show()

