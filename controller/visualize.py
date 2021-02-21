import fft_histogram
import matplotlib
import matplotlib.pyplot as plt
import numpy as np

fig, ax = plt.subplots()
ax.grid()
plt.show()




while 1:
    y = fft_histogram.histogram()
    x = range(len(y))
    ax.cla()
    ax.plot(x,y)
    plt.draw()
