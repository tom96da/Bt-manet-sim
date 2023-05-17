'''
   file: somCSV.py
   author: tom96da
   brief: 複数csvからプロット
   date: 2023-05-17
'''

import matplotlib.pyplot as plt
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
ax.set_title("デバイスの移動",size=15)

for i in range(0,3):
    ax.plot(x[i], y[i],"o")

ax.set_xlabel("x", size=10)
ax.set_ylabel("y", size=10)
ax.set_xlim(-5, 5)
ax.set_ylim(-5, 5)
 

plt.show()
