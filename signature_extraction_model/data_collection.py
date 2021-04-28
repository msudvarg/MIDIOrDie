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
    time.sleep(0.3)
    print(sf2)

profile_name = input("Enter name to give guitar profile: ")

input("Verify your guitar is plugged in and the correct mic is set and press enter to continue")

frames = []
labels = []

# =================================== COLLECTING SILENCE PROFILE ==================================================
print("Testing silence (jk, it's zero, change the input if you need)")
j = 0
stream = pa.open(format=pyaudio.paFloat32, channels=1, rate=44100, input=True, output=False, frames_per_buffer=LEN)
while stream.is_active() and j < RECORD_DURATION * 10:
    data = stream.read(LEN)
    data = np.frombuffer(data, dtype=np.float32)
    data = np.multiply(data, signal.windows.hamming(LEN))
    fft_arr = fft(data)[:int(LEN / 2)]
    fft_arr = [math.sqrt(x.real ** 2 + x.imag ** 2) for x in fft_arr]
    fft_arr = [20 * math.log10(x) if x > 0 else -100 for x in fft_arr]
    frames.append(fft_arr[:INPUTS])
    #labels.append(np.zeros((OUTPUTS), dtype=float))
    j += 1
stream.close()

stacked = []
stacked = np.stack(frames, axis=0)
silence = np.mean(stacked, axis=0)


# # Plot silence
# plt.plot(silence)
# plt.show()

frames = []
# frames = [f - silence for f in frames[::2]]
# labels = labels[::2]


soundfonts = [('1115-JazzMelodic.sf2', 0, 3), ('198-Dsix magic 20.9 MB.sf2', 0, 1), 
            ('198-VL-1 Eguitar VS 17MB.sf2', 0, 2), ('198-Voxline Session.sf2',0,2), ('336-saz.sf2',0,2), 
            ('4043-Tholen5.sf2', 2, 5), ('459-CrujienteHevy guitar.sf2',0,1), 
            ('459-GuitarSetPasisHeavyAndClean.sf2',2,5), ('463-Jazz Guitar.sf2',0,6),
            ('754-Donnys Guitar.SF2',0,1), ('Acoustic Gtr.sf2',0,6), ('FluidR3_GM.sf2',24,28), 
            ('Guitar Acoustic (963KB).sf2',0,9), ('Guitar Acoustic.SF2',0,1), ('Guitar Distortion.SF2',0,1), 
            ('Heavy.sf2',0,5), ('Modern Guitar.sf2',0,3), ('Nylon Guitar.sf2',0,4), 
            ('Rock Gitaro.sf2',0,1), ('Studio FG460s II Pro Guitar Pack.SF2',0,6), ('The Guitar.sf2',0,1),
            ('classical_guitar_1a.sf3',0,1)]

for sf2, lower, upper in soundfonts:
    for program in range(lower, upper):

        prompts = [("Let's start on the low E string", 0), ("Moving onto the A string", 5), ("Moving onto the D string", 10),
                ("Moving onto the G string", 15), ("Moving onto the B string", 19), ("Finally the high E string", 24)]

        single_note_frames = []
        single_note_labels = []
        for p, i in prompts:
            #print(p)
            #input("Press enter to start")

            for j in range(18):
                play_chord([i+j], sf2, program)

                l = np.zeros((OUTPUTS), dtype=float)
                l[i+j] = 1

                # Read for a second
                k = 0
                stream = pa.open(format=pyaudio.paFloat32, channels=1, rate=44100, input=True, output=False, frames_per_buffer=LEN)
                while stream.is_active() and k < RECORD_DURATION:
                    data = stream.read(LEN)
                    data = np.frombuffer(data, dtype=np.float32)
                    data = np.multiply(data, signal.windows.hamming(LEN))
                    fft_arr = fft(data)[:int(LEN/2)]
                    fft_arr = [math.sqrt(x.real**2 + x.imag**2) for x in fft_arr]
                    fft_arr = [20 * math.log10(x) for x in fft_arr]
                    single_note_frames.append(fft_arr[:INPUTS])
                    single_note_labels.append(l)
                    k+=1

                stream.close()

                # # Plot note
                # plt.plot(single_note_frames[-24])
                # plt.show()

                time.sleep(0.5)



        # Build calibration
        STANDARD_FREQ = 250

        def note_to_freq(note):
            return 82.41 * math.pow(1.059463094, note)

        standardized_bass = []
        standardized_mid = []
        standardized_treble = []

        for f, l in zip(single_note_frames, single_note_labels):
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
        #bass /= bass.max()
        stacked = np.stack(standardized_mid, axis=0)
        mid = np.mean(stacked, axis=0)
        #mid /= stacked.max()
        stacked = np.stack(standardized_treble, axis=0)
        treble = np.mean(stacked, axis=0)
        #treble /= stacked.max()

        calibrations = np.stack([silence, bass, mid, treble], axis=0)

        print("Saving calibration to: " + profile_name + "_calib.npy")
        np.save(profile_name + "_calib.npy", calibrations)

        # Save a random sample for training
        print("Packing intermediary data")
        random_sample = np.random.randint(RECORD_DURATION)
        frames += single_note_frames[RECORD_DURATION-random_sample::RECORD_DURATION] #+ frames[1::RECORD_DURATION]
        labels += single_note_labels[RECORD_DURATION-random_sample::RECORD_DURATION]
        frames_stacked = np.stack(frames, axis=0)
        labels_stacked = np.stack(labels, axis=0)

        np.save(profile_name + "_inputs.npy", np.stack([np.concatenate((np.expand_dims(f,axis=0),calibrations[1:])) for f in frames], axis=0))
        np.save(profile_name + "_labels.npy", labels_stacked)

        # Chords
        chords = [[0,0,2,2,2,0], [0,0,2,2,1,0], [-1,3,2,0,1,0], [-1,0,0,2,3,2], [-1,0,0,2,1,2], [0,2,2,1,0,0],
                  [0,2,2,0,0,0], [-1,-1,3,2,1,1],[3,2,0,0,3,3], [-1,0,2,0,2,0], [-1,0,2,0,1,0], [-1,2,1,2,0,2],
                  [-1,3,2,3,1,0]]


        for i, chord in enumerate(chords):
            note_list = []

            # Build label from chord signature
            l = np.zeros((OUTPUTS), dtype=float)
            if chord[0] >= 0:
                l[0 + chord[0]] = 1
                note_list.append(0 + chord[0])
            if chord[1] >= 0:
                l[5 + chord[1]] = 1
                note_list.append(5 + chord[1])
            if chord[2] >= 0:
                l[10 + chord[2]] = 1
                note_list.append(10 + chord[2])
            if chord[3] >= 0:
                l[15 + chord[3]] = 1
                note_list.append(15 + chord[3])
            if chord[4] >= 0:
                l[19 + chord[4]] = 1
                note_list.append(19 + chord[4])
            if chord[5] >= 0:
                l[24 + chord[5]] = 1
                note_list.append(24 + chord[5])

            play_chord(note_list, sf2, program)

            j = 0
            stream = pa.open(format=pyaudio.paFloat32, channels=1, rate=44100, input=True, output=False, frames_per_buffer=LEN)
            random_sample = np.random.randint(RECORD_DURATION)
            while stream.is_active() and j < RECORD_DURATION:
                data = stream.read(LEN)

                #if j == 1 or j == RECORD_DURATION - 1:
                if j == random_sample:
                    # Save last datapoint (cleanest) and 2nd datapoint (attack) for testing later
                    data = np.frombuffer(data, dtype=np.float32)
                    data = np.multiply(data, signal.windows.hamming(LEN))
                    fft_arr = fft(data)[:int(LEN / 2)]
                    fft_arr = [math.sqrt(x.real ** 2 + x.imag ** 2) for x in fft_arr]
                    fft_arr = [20 * math.log10(x) for x in fft_arr]
                    frames.append(fft_arr[:INPUTS] - silence)
                    labels.append(l)
                j += 1
            stream.close()

            time.sleep(0.5)
            # # Plot note
            # stacked = np.stack(frames[-75:], axis=0)
            # avg = np.mean(stacked, axis=0)
            # plt.plot(avg)
            # plt.show()

        # Save the last (ie cleanest) sample from each note for training later
        print("Packing intermediary data")
        frames_stacked = np.stack(frames, axis=0)
        labels_stacked = np.stack(labels, axis=0)

        np.save(profile_name + "_inputs.npy", np.stack([np.concatenate((np.expand_dims(f,axis=0),calibrations[1:])) for f in frames], axis=0))
        np.save(profile_name + "_labels.npy", labels_stacked)


        for i in range(100):
            note_list = []

            # Generate random chord, construct string and label
            start_string = np.random.randint(5)
            end_string = start_string + 1
            three_string = np.random.randint(3) == 0
            if start_string == 4 and three_string:
                start_string = 3
            elif three_string:
                end_string += 1
            chord = [-1] * 6
            capo = np.random.randint(15)
            for j in range(6):
                if j < start_string or j > end_string:
                    chord[j] = -1
                else:
                    chord[j] = capo + np.random.randint(4)
            
            # Build label from chord signature
            l = np.zeros((OUTPUTS), dtype=float)
            if chord[0] >= 0:
                l[0 + chord[0]] = 1
                note_list.append(0 + chord[0])
            if chord[1] >= 0:
                l[5 + chord[1]] = 1
                note_list.append(5 + chord[1])
            if chord[2] >= 0:
                l[10 + chord[2]] = 1
                note_list.append(10 + chord[2])
            if chord[3] >= 0:
                l[15 + chord[3]] = 1
                note_list.append(15 + chord[3])
            if chord[4] >= 0:
                l[19 + chord[4]] = 1
                note_list.append(19 + chord[4])
            if chord[5] >= 0:
                l[24 + chord[5]] = 1
                note_list.append(24 + chord[5])

            play_chord(note_list, sf2, program)

            j = 0
            stream = pa.open(format=pyaudio.paFloat32, channels=1, rate=44100, input=True, output=False, frames_per_buffer=LEN)
            random_sample = np.random.randint(RECORD_DURATION)
            while stream.is_active() and j < RECORD_DURATION:
                data = stream.read(LEN)

                #if j == 1 or j == RECORD_DURATION - 1:
                if j == random_sample:
                    # Save last datapoint (cleanest) and 2nd datapoint (attack) for testing later
                    data = np.frombuffer(data, dtype=np.float32)
                    data = np.multiply(data, signal.windows.hamming(LEN))
                    fft_arr = fft(data)[:int(LEN / 2)]
                    fft_arr = [math.sqrt(x.real ** 2 + x.imag ** 2) for x in fft_arr]
                    fft_arr = [20 * math.log10(x) for x in fft_arr]
                    frames.append(fft_arr[:INPUTS] - silence)
                    labels.append(l)

                j += 1
            stream.close()
            time.sleep(0.5)

        print("Packing final data")
        frames_stacked = np.stack(frames, axis=0)
        labels_stacked = np.stack(labels, axis=0)

        np.save(profile_name + "_inputs.npy", np.stack([np.concatenate((np.expand_dims(f,axis=0),calibrations[1:])) for f in frames], axis=0))
        np.save(profile_name + "_labels.npy", labels_stacked)


print("Done")

# close PyAudio (7)
pa.terminate()