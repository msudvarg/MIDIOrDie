import pyaudio
import time
import numpy as np
from scipy.fft import fft, fftshift
from scipy import signal
import math

from matplotlib import pyplot as plt

pa = pyaudio.PyAudio()

LEN = 1764
OUTPUTS = 48

# def callback(in_data, frame_count, time_info, status):
#     data = np.fromstring(in_data, dtype=np.float32)
#
#     return (None, pyaudio.paContinue)


prompts = ["Play an open low E string", "Play an E string, 1st fret", "Play an E string, 2nd fret", "Play an E string, 3rd fret", "Play an E string, 4th fret",
           "Play an open A string", "Play an A string, 1st fret", "Play an A string, 2nd fret", "Play an A string, 3rd fret", "Play an A string, 4th fret",
           "Play an open D string", "Play a D string, 1st fret", "Play a D string, 2nd fret", "Play a D string, 3rd fret", "Play a D string, 4th fret",
           "Play an open G string", "Play a G string, 1st fret", "Play a G string, 2nd fret", "Play a G string, 3rd fret",
           "Play an open B string", "Play a B string, 1st fret", "Play a B string, 2nd fret", "Play a B string, 3rd fret", "Play a B string, 4th fret",
           "Play an open high E string", "Play an E string, 1st fret", "Play an E string, 2nd fret", "Play an E string, 3rd fret", "Play an E string, 4th fret"]

frames=[]
labels=[]

profile_name = input("Enter name to give guitar profile: ")

print("Need complete silence starting in...")
for j in range(3,0,-1):
    print(j)
    time.sleep(1)
print("Now")
silence = []
j = 0
stream = pa.open(format=pyaudio.paFloat32, channels=1, rate=44100, input=True, output=False, frames_per_buffer=LEN)
while stream.is_active() and j < 75:
    data = stream.read(LEN)
    data = np.frombuffer(data, dtype=np.float32)
    data = np.multiply(data, signal.windows.hamming(LEN))
    fft_arr = fft(data)[:int(LEN / 2)]
    fft_arr = [math.sqrt(x.real ** 2 + x.imag ** 2) for x in fft_arr]
    fft_arr = [20 * math.log10(x) for x in fft_arr]
    silence.append(fft_arr[:100])
    j += 1
stream.close()
stacked = []
stacked = np.stack(silence[-75:], axis=0)
silence = np.mean(stacked, axis=0)

for i, p in enumerate(prompts):
    print(p)
    #input("Press any key when ready")
    for j in range(3,0,-1):
        print(j)
        time.sleep(1)
    print("Now")
    l = np.zeros((OUTPUTS), dtype=float)
    l[i] = 1
    j = 0
    stream = pa.open(format=pyaudio.paFloat32, channels=1, rate=44100, input=True, output=False, frames_per_buffer=LEN)
    while stream.is_active() and j < 75:
        data = stream.read(LEN)
        data = np.frombuffer(data, dtype=np.float32)
        data = np.multiply(data, signal.windows.hamming(LEN))
        fft_arr = fft(data)[:int(LEN/2)]
        fft_arr = [math.sqrt(x.real**2 + x.imag**2) for x in fft_arr]
        fft_arr = [20 * math.log10(x) for x in fft_arr]
        frames.append(fft_arr[:100] - silence)
        labels.append(l)
        j+=1
    stream.close()

    # Plot note
    stacked = np.stack(frames[-75:], axis=0)
    avg = np.mean(stacked, axis=0)
    plt.plot(avg)
    plt.show()

frames = np.stack(frames, axis=0)
labels = np.stack(labels, axis=0)

np.save(profile_name + "_row" + str(int(LEN/2)) + ".npy", frames)
np.save(profile_name + "_labels" + "_row" + str(int(OUTPUTS)) + ".npy", labels)

# close PyAudio (7)
pa.terminate()