import fft_histogram
import matplotlib
import matplotlib.pyplot as plt

plt.show()

while 1:
    y = fft_histogram.histogram()
    x = range(len(y))
    plt.cla()
    plt.plot(x,y)
    plt.draw()
    plt.pause(0.1)
