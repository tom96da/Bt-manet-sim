import matplotlib.pyplot as plt
import math

num_dev = 100
field_size = 60
distance = 6.7

pos_x = [0]
pos_y = [0]

dx = round(distance * math.cos(math.pi / 3), 2)
dy = round(distance * math.sin(math.pi / 3), 2)

direction = 1
while True:
    while True:
        pos_x.append(pos_x[-1] + direction * distance)
        pos_y.append(pos_y[-1])
        if pos_x[-1] > field_size or pos_x[-1] < 0:
            pos_x.pop(-1)
            pos_y.pop(-1)
            pos_x.append(pos_x[-1] + direction * dx)
            pos_y.append(pos_y[-1] + dy)
            direction = -direction
            if pos_x[-1] > field_size or pos_x[-1] < 0:
                pos_x.pop(-1)
                pos_y.pop(-1)
                pos_x.append(pos_x[-1] + direction * dx)
                pos_y.append(pos_y[-1] + dy)
            break
    if pos_y[-1] > field_size:
        pos_x.pop(-1)
        pos_y.pop(-1)
        break

print(len(pos_x), ' devices.')

fig = plt.figure(figsize=(6, 5))
ax = fig.add_subplot(111)
ax.set_aspect('equal')
ax.tick_params(labelsize=12)
ax.set_title("Position", size=18, fontweight='bold')
ax.set_xlabel("x", size=18, fontweight='bold')
ax.set_ylabel("y", size=18, fontweight='bold')
ax.set_xlim(0 - 2, field_size + 2)
ax.set_ylim(0 - 2, field_size + 2)

ax.plot(pos_x, pos_y, "o", c='k', ms=3)

plt.show()
