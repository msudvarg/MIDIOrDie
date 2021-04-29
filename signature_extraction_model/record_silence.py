import pyaudio
import time
import numpy as np
from scipy.fft import fft, fftshift
from scipy import signal
import math
import re
import os
from midiutil import MIDIFile

from matplotlib import pyplot as plt

pa = pyaudio.PyAudio()

LEN = 1764
INPUTS = 80
OUTPUTS = 48
RECORD_DURATION = 25    # 25 increments of 40ms each = 1s

def play_chord(notes, sf2, program):
    notes = [n + 40 for n in notes]
    t     = 0    # In beats
    duration = 1.5    # In beats
    tempo    = 60   # In BPM
    MyMIDI = MIDIFile(1)  # One track, defaults to format 1 (tempo track is created
                        # automatically)
    MyMIDI.addTempo(0, t, tempo)
    MyMIDI.addProgramChange(0, 0, 0, program)		# Number is soundfount number
    for i, pitch in enumerate(notes):
        MyMIDI.addNote(0, 0, pitch, t + 0.02*i, duration, np.random.randint(80,120))
    with open("chord.mid", "wb") as output_file:
        MyMIDI.writeFile(output_file)
    time.sleep(0.1)
    os.system('fluidsynth --no-shell -a pulseaudio "sf2/%s" chord.mid &>/dev/null &' % (sf2))
    print(sf2)

profile_name = input("Enter name to give guitar profile: ")

input("Verify your guitar is plugged in and the correct mic is set and press enter to continue")

frames = []
labels = []

calibrations = [np.load("automated_guitars_2_calib.npy"), np.load("automated_guitars_3_calib.npy"), np.load("automated_guitars_calib.npy"), np.load("dane_take1_calib.npy")]


for calib in calibrations:
    print("Testing silence (jk, it's zero, change the input if you need)")
    j = 0
    stream = pa.open(format=pyaudio.paFloat32, channels=1, rate=44100, input=True, output=False, frames_per_buffer=LEN)
    while stream.is_active() and j < RECORD_DURATION * 5:
        data = stream.read(LEN)
        data = np.frombuffer(data, dtype=np.float32)
        data = np.multiply(data, signal.windows.hamming(LEN))
        fft_arr = fft(data)[:int(LEN / 2)]
        fft_arr = [math.sqrt(x.real ** 2 + x.imag ** 2) for x in fft_arr]
        fft_arr = [20 * math.log10(x) if x > 0 else -100 for x in fft_arr]
        frames.append(fft_arr[:INPUTS])
        labels.append(np.zeros((OUTPUTS), dtype=float))
        j += 1
    stream.close()

    stacked = []
    stacked = np.stack(frames, axis=0)
    silence = np.mean(stacked, axis=0)

    np.save(profile_name + "_inputs.npy", np.stack([np.concatenate((np.expand_dims(f,axis=0),calib[1:])) for f in frames], axis=0))
    np.save(profile_name + "_labels.npy", np.stack(labels, axis=0))