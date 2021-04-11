import pyaudio
import time
import numpy as np
from scipy.fft import fft, fftshift
from scipy import signal
import math
import re

from matplotlib import pyplot as plt

pa = pyaudio.PyAudio()

LEN = 1764
INPUTS = 80
OUTPUTS = 48

profile_name = input("Enter name to give guitar profile: ")

input("Verify your guitar is plugged in and the correct mic is set and press enter to continue")

frames = []
labels = []

# =================================== COLLECTING SILENCE PROFILE ==================================================
print("Need complete silence starting in...")
for j in range(3, 0, -1):
    print(j)
    time.sleep(1)
print("Now")
j = 0
stream = pa.open(format=pyaudio.paFloat32, channels=1, rate=44100, input=True, output=False, frames_per_buffer=LEN)
while stream.is_active() and j < 75:
    data = stream.read(LEN)
    data = np.frombuffer(data, dtype=np.float32)
    data = np.multiply(data, signal.windows.hamming(LEN))
    fft_arr = fft(data)[:int(LEN / 2)]
    fft_arr = [math.sqrt(x.real ** 2 + x.imag ** 2) for x in fft_arr]
    fft_arr = [20 * math.log10(x) for x in fft_arr]
    frames.append(fft_arr[:INPUTS])
    labels.append(np.zeros((OUTPUTS), dtype=float))
    j += 1
stream.close()

stacked = []
stacked = np.stack(frames, axis=0)
silence = np.mean(stacked, axis=0)

frames = [f - silence for f in frames]

# ================================ SINGLE NOTE DATA COLLECTION =========================================
c = input("Do you want to perform single note data collection? (if you decline, you'll need to specify you calibration file) (y or n)")
if c == 'y':

    print("You will progress through all 6 strings, incrementally playing an open string up to the 9th fret on each "
          "string. You will get a 3-2-1-Now countdown for each fret, and audio will be recorded for 1 second before "
          "moving onto the next fret. You will have the option to pause between strings")

    prompts = [("Let's start on the low E string", 0), ("Moving onto the A string", 5), ("Moving onto the D string", 10),
               ("Moving onto the G string", 15), ("Moving onto the B string", 19), ("Finally the high E string", 24)]

    for p, i in prompts:
        print(p)
        input("Press enter to start")

        for j in range(10):
            print("Fret " + str(j))
            #input("Press any key when ready")
            time.sleep(1)
            for k in range(3,0,-1):
                print(k)
                time.sleep(1)
            print("Now")
            l = np.zeros((OUTPUTS), dtype=float)
            l[i+j] = 1
            k = 0
            stream = pa.open(format=pyaudio.paFloat32, channels=1, rate=44100, input=True, output=False, frames_per_buffer=LEN)
            while stream.is_active() and k < 25:
                data = stream.read(LEN)
                data = np.frombuffer(data, dtype=np.float32)
                data = np.multiply(data, signal.windows.hamming(LEN))
                fft_arr = fft(data)[:int(LEN/2)]
                fft_arr = [math.sqrt(x.real**2 + x.imag**2) for x in fft_arr]
                fft_arr = [20 * math.log10(x) for x in fft_arr]
                frames.append(fft_arr[:INPUTS] - silence)
                labels.append(l)
                k+=1
            stream.close()

            # # Plot note
            # stacked = np.stack(frames[-75:], axis=0)
            # avg = np.mean(stacked, axis=0)
            # plt.plot(avg)
            # plt.show()



    # Build calibration
    STANDARD_FREQ = 250

    def note_to_freq(note):
        return 82.41 * math.pow(1.059463094, note)

    standardized_bass = []
    standardized_mid = []
    standardized_treble = []

    for f, l in zip(frames, labels):
        note = np.where(l == 1)[0]
        if len(note) == 0:
            continue
        freq = note_to_freq(note)
        interp_factor = freq / STANDARD_FREQ
        interp = np.interp(np.linspace(0, interp_factor * INPUTS, INPUTS), np.linspace(0, INPUTS, INPUTS), f[:INPUTS])

        if (note < 10):
            standardized_bass.append(interp)
        elif(note < 19):
            standardized_mid.append(interp)
        else:
            standardized_treble.append(interp)

    # Plot note
    stacked = np.stack(standardized_bass, axis=0)
    bass = np.mean(stacked, axis=0)
    bass /= bass.max()
    stacked = np.stack(standardized_mid, axis=0)
    mid = np.mean(stacked, axis=0)
    mid /= stacked.max()
    stacked = np.stack(standardized_treble, axis=0)
    treble = np.mean(stacked, axis=0)
    treble /= stacked.max()

    calibrations = np.stack([silence, bass, mid, treble], axis=0)

    np.save(profile_name + "_calib_row" + str(OUTPUTS) + ".npy", calibrations)

    print("Packing intermediary data")
    frames_stacked = np.stack(frames, axis=0)
    labels_stacked = np.stack(labels, axis=0)

    np.save(profile_name + "_row" + str(INPUTS) + ".npy", frames_stacked)
    np.save(profile_name + "_labels" + "_row" + str(int(OUTPUTS)) + ".npy", labels_stacked)

else:
    calib_filename = input("Enter name of calibration file (eg guitarname_calib_row48.npy): ")
    calibrations = np.load(calib_filename)


# ================================ Chord data collection =======================================
c = input("Do you want to perform chord data collection? (y or n)")
if c == 'y':
    prompts = ["Play an A chord (002220)", "Play an Am chord (002210)", "Play a C chord (X32010)",
               "Play a D chord (X00232)", "Play a D7 chord (X00212)", "Play a E chord (022100)",
               "Play an Em chord (022000)", "Play an F chord (xx3211)", "Play a G chord (320033)",
               "Play an A7 chord (x02020)", "Play an Am7 chord (x02010)", "Play B7 chord (x21202)",
               "Play a C7 chord (x32310)"]


    for i, p in enumerate(prompts):
        print(p)
        input("Press enter when ready")

        for j in range(3, 0, -1):
            print(j)
            time.sleep(1)
        print("Now")

        # Build label from chord signature
        l = np.zeros((OUTPUTS), dtype=float)
        chord = p[-7:-1]
        if re.search('[0-9]', chord[0]):
            l[0 + int(chord[0])] = 1
        if re.search('[0-9]', chord[1]):
            l[5 + int(chord[1])] = 1
        if re.search('[0-9]', chord[2]):
            l[10 + int(chord[2])] = 1
        if re.search('[0-9]', chord[3]):
            l[15 + int(chord[3])] = 1
        if re.search('[0-9]', chord[4]):
            l[19 + int(chord[4])] = 1
        if re.search('[0-9]', chord[5]):
            l[24 + int(chord[5])] = 1


        j = 0
        stream = pa.open(format=pyaudio.paFloat32, channels=1, rate=44100, input=True, output=False,
                         frames_per_buffer=LEN)
        while stream.is_active() and j < 25:
            data = stream.read(LEN)
            data = np.frombuffer(data, dtype=np.float32)
            data = np.multiply(data, signal.windows.hamming(LEN))
            fft_arr = fft(data)[:int(LEN / 2)]
            fft_arr = [math.sqrt(x.real ** 2 + x.imag ** 2) for x in fft_arr]
            fft_arr = [20 * math.log10(x) for x in fft_arr]
            frames.append(fft_arr[:INPUTS] - silence)
            labels.append(l)
            j += 1
        stream.close()

        # # Plot note
        # stacked = np.stack(frames[-75:], axis=0)
        # avg = np.mean(stacked, axis=0)
        # plt.plot(avg)
        # plt.show()

# ======================= Random intervals ===================================
c = input("Do you want to perform random interval data collection? (y or n)")
if c == 'y':
    print("Psych, not implemented yet")
    time.sleep(1)

# ======================== Pack and output dataset =================================
print("Packing data")
frames = np.stack(frames, axis=0)
labels = np.stack(labels, axis=0)

np.save(profile_name + "_row" + str(INPUTS) + ".npy", frames)
np.save(profile_name + "_labels" + "_row" + str(int(OUTPUTS)) + ".npy", labels)


print("Done")

# close PyAudio (7)
pa.terminate()