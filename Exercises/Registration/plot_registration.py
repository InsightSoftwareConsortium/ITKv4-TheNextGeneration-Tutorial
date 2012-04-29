#!/usr/bin/env python

from matplotlib import pyplot as plt

metric_value = []
rotation = []
x_translation = []
y_translation = []

with open('output.txt', 'r') as f:
    for line in f.readlines():
        split_line = line.split(':')
        if split_line[0].strip() == 'Metric value':
            metric_value.append(float(split_line[1]))
        elif split_line[0].strip() == 'Transform parameters':
            params_array = split_line[1].strip()[1:-1].split(',')
            rotation.append(float(params_array[0]))
            x_translation.append(float(params_array[1]))
            y_translation.append(float(params_array[2]))

fig = plt.figure(1, figsize=(14, 3))
ax1 = fig.add_subplot(131)
ax1.plot(metric_value)
ax1.set_xlabel('iteration')
ax1.set_ylabel('metric value')

ax2 = fig.add_subplot(132)
ax2.plot(rotation)
ax2.set_xlabel('iteration')
ax2.set_ylabel('rotation')

ax3 = fig.add_subplot(133)
ax3.plot(x_translation)
ax3.plot(y_translation)
ax3.set_xlabel('iteration')
ax3.set_ylabel('translations')

plt.show()
