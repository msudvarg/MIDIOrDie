import fft_histogram
import matplotlib
import matplotlib.pyplot as plt

scale = int(20) #Only plot 1 out of every scale datapoints

plt.show()
plt.ion()

while 1:
    y_full = fft_histogram.histogram()
    x = range(int(len(y_full)/scale))
    y = []
    if scale > 1:
        for i in x:
            y.append(y_full[i*scale])
    else:
        y = y_full
    plt.cla()
    plt.plot(x,y)
    plt.gcf().canvas.draw_idle()
    plt.gcf().canvas.start_event_loop(0.1)
