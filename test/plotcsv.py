'''
   file: plotcsv.py
   author: tom96da
   brief: csvからデータ取得とプロット
   date: 2023-05-17
'''

import matplotlib.pyplot as plt
import pandas as pd

plt.rcParams['font.family'] = 'Source Han Code JP'

# CSVファイルのパス
csv_file = 'test/data/data0.csv'

# CSVファイルの読み込み
data = pd.read_csv(csv_file)

# x座標とy座標のデータを抽出
x = data['x']
y = data['y']

# グラフの描画
fig = plt.figure(figsize=(5.0, 5.0))
ax = fig.add_subplot(111)
ax.scatter(x, y, marker='o',s=1)
ax.set_title("XY座標", size=20)
ax.set_xlabel("x", size=15)
ax.set_ylabel("y", size=15)
ax.set_xlim(-5, 5)
ax.set_ylim(-5, 5)
 
plt.show()
