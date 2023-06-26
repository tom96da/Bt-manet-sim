import matplotlib.pyplot as plt
import math

num_dev = 100
field_size = 30
distance = 5

pos_x = []
pos_y = []

pos_x.append(0)
pos_y.append(0)

for arg in range(6):
    radius = distance
    theta = arg * math.pi / 3
    pos_x.append(radius * math.cos(theta))
    pos_y.append(radius * math.sin(theta))


for group in range(6):
    theta = group * math.pi / 3
    pos_x.append(pos_x[len(pos_x) - 6] + distance * math.cos(theta))
    pos_y.append(pos_y[len(pos_y) - 6] + distance * math.sin(theta))

for group in range(6):
    theta = (2 + group) * math.pi / 3
    pos_x.append(pos_x[len(pos_x) - 6] + distance * math.cos(theta))
    pos_y.append(pos_y[len(pos_y) - 6] + distance * math.sin(theta))

for group in range(6):
    theta = group * math.pi / 3
    pos_x.append(pos_x[len(pos_x) - 6] + distance * math.cos(theta))
    pos_y.append(pos_y[len(pos_y) - 6] + distance * math.sin(theta))

for group in range(12):
    theta = (2 + group) * math.pi / 3
    pos_x.append(pos_x[len(pos_x) - 6] + distance * math.cos(theta))
    pos_y.append(pos_y[len(pos_y) - 6] + distance * math.sin(theta))

for group in range(6):
    theta = group * math.pi / 3
    pos_x.append(pos_x[len(pos_x) - 6] + distance * math.cos(theta))
    pos_y.append(pos_y[len(pos_y) - 6] + distance * math.sin(theta))

for group in range(18):
    theta = (-1 + group) * math.pi / 3
    pos_x.append(pos_x[len(pos_x) - 6] + distance * math.cos(theta))
    pos_y.append(pos_y[len(pos_y) - 6] + distance * math.sin(theta))

for group in range(6):
    theta = (1 + group) * math.pi / 3
    pos_x.append(pos_x[len(pos_x) - 12] + distance * math.cos(theta))
    pos_y.append(pos_y[len(pos_y) - 12] + distance * math.sin(theta))

for group in range(6):
    theta = (2 + group) * math.pi / 3
    pos_x.append(pos_x[len(pos_x) - 6] + distance * math.cos(theta))
    pos_y.append(pos_y[len(pos_y) - 6] + distance * math.sin(theta))

for group in range(6):
    theta = (-1 + group) * math.pi / 3
    pos_x.append(pos_x[len(pos_x) - 12] + distance * math.cos(theta))
    pos_y.append(pos_y[len(pos_y) - 12] + distance * math.sin(theta))

for group in range(6):
    theta = (2 + group) * math.pi / 3
    pos_x.append(pos_x[len(pos_x) - 12] + distance * math.cos(theta))
    pos_y.append(pos_y[len(pos_y) - 12] + distance * math.sin(theta))

for group in range(6):
    theta = (-1 + group) * math.pi / 3
    pos_x.append(pos_x[len(pos_x) - 12] + distance * math.cos(theta))
    pos_y.append(pos_y[len(pos_y) - 12] + distance * math.sin(theta))

for group in range(6):
    theta = group * math.pi / 3
    pos_x.append(pos_x[len(pos_x) - 24] + distance * math.cos(theta))
    pos_y.append(pos_y[len(pos_y) - 24] + distance * math.sin(theta))

for group in range(6):
    theta = (-1 + group) * math.pi / 3
    pos_x.append(pos_x[len(pos_x) - 6] + distance * math.cos(theta))
    pos_y.append(pos_y[len(pos_y) - 6] + distance * math.sin(theta))

for group in range(6):
    theta = (2 + group) * math.pi / 3
    pos_x.append(pos_x[len(pos_x) - 12] + distance * math.cos(theta))
    pos_y.append(pos_y[len(pos_y) - 12] + distance * math.sin(theta))

print(len(pos_x), 'devices')


fig = plt.figure(figsize=(6, 5))
ax = fig.add_subplot(111)
ax.set_aspect('equal')
ax.tick_params(labelsize=12)
ax.set_title("Position", size=18, fontweight='bold')
ax.set_xlabel("x", size=18, fontweight='bold')
ax.set_ylabel("y", size=18, fontweight='bold')
ax.set_xlim(-field_size, field_size)
ax.set_ylim(-field_size, field_size)

ax.plot(pos_x, pos_y, "o", c='k', ms=3)
ax.plot(pos_x[len(pos_x)-6], pos_y[len(pos_x)-6], "o", c='c', ms=3)
ax.plot(pos_x[len(pos_x)-1], pos_y[len(pos_x)-1], "o", c='r', ms=3)

plt.show()
