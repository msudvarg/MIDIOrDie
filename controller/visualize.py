import fft_histogram
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import sys


scale = int(20) #Only plot 1 out of every scale datapoints

plt.show()
plt.ion()

while 1:
    y_full = fft_histogram.histogram()

    x = np.linspace(0, fft_histogram.max_hz(), fft_histogram.bin_count())
    x = [int(v) for v in x]
    bucket_size = fft_histogram.max_hz() / fft_histogram.bin_count()
        
    y = []
    y = y_full
    plt.cla()
    plt.ylim(0, 1500)
    plt.plot(x,y)
    plt.gcf().canvas.draw_idle()
    plt.gcf().canvas.start_event_loop(0.1)
