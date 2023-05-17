'''
   file: someAni.py
   author: tom96da
   brief: csvをアニメーション化
   date: 2023-05-17
'''

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import pandas as pd

plt.rcParams['font.family'] = 'Source Han Code JP'

fname = []
x = []
y = []

for i in range(0,3):
    fname.append("test/data/data" + str(i) +".csv")
    data = pd.read_csv(fname[i])

    x.append(data['x'])
    y.append(data['y'])

fig = plt.figure(figsize=(5,5))
ax = fig.add_subplot(111)

def update(frame):
    ax.cla()
    ax.set_title("デバイスアニメーション",size=15)
    ax.set_xlabel("x", size=10)
    ax.set_ylabel("y", size=10)
    ax.set_xlim(-5, 5)
    ax.set_ylim(-5, 5)
    ax.plot(x[frame], y[frame], "o",color='black')


anim = FuncAnimation(fig, update, frames=range(3), interval=500)

plt.show()
anim.save("test/fig/someAni.gif", writer="imagemagick")
plt.close
