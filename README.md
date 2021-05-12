# Compiliation

Retrieve this repo with

    git clone https://github.com/msudvarg/MIDIOrDie
    cd MIDIOrDie
    git submodule init
    git submodule update

Acquire dependencies and build the controller with

    sudo apt install -y libportaudiocpp0 libfftw3-dev
    mkdir build && cd build
    cmake ..
    make

Acquire Tensorflow dependencies and build the server with

    # Install portaudio again, if you're running on a different machine
    sudo apt install -y libportaudiocpp0
    mkdir build && cd build

    # Install prereqs and bazel
    sudo apt-get install cmake curl g++-7 git python3-dev python3-numpy sudo wget
    curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor > bazel.gpg
    sudo mv bazel.gpg /etc/apt/trusted.gpg.d/
    echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | sudo tee /etc/apt/sources.list.d/bazel.list
    sudo apt-get update && sudo apt-get install bazel-3.1.0

    # Clone a helpful repo from FloopCZ to install the API
    git clone https://github.com/FloopCZ/tensorflow_cc.git
    cd tensorflow_cc/tensorflow_cc
    mkdir build && cd build
    cmake -DALLOW_CUDA=off ..
    make
    sudo make install

    # Build the server
    cmake -DBUILD_SERVER=true ..
    make

# Running

Run the server from the build directory on the host machine like so:

    ./server -p 1 -a

Run the controller from build directory on the controller device like so:

    ./controller -f -i 192.168.x.x

If the controller is running on the same machine as the host, you can omit the IP address

    ./controller -f

Detailed description of command line options are below
## Command Line Tools
### Controller
The controller collects audio data from the audio input and performs the fast Fourier transform. It searches for a server socket, and send the data over the wire.

Its options are as follows
- i \<ip-addr\>: Set the ip address to look for a host under. Defaults to 127.0.0.1, the local loopback IP
- f: Run forever. Omitting this flag will run for 1000 iterations

### Server
The server is a proxy for the host application. It acts as a server and channel broker for the controllers. It performs the translation from the fourier series sent from the controller to Midi notes, and sends the midi notes to the host application on the designated port, one Midi channel per instrument.

Its options are as follows
- p \<port-num\>: This designates the midi port on which to send the notes. Defaults to 0, the hardware midi port. If you are unsure of which port to use, the server will print all known ports and the names of the application using them when it initializes
- a: Print all midi notes detected. If absent the program will only send the Midi note with the lowest frequency
- d: If this option is present, the first controller will be mapped to Midi channel 10, which is the drum channel. If absent, the broker will begin mapping channels at 1.
- c: This activates hillclimbing mode, which detects local maxima on the Fourier series and translates them to midi notes. If it is absent, the server will use the signature recognition.

# Constants

Constants related to the FFT are defined as (`constexpr`) values in `include/manifest.h`

* `int SAMPLE_RATE`: 44100 Hz sample rate
* `int WINDOW_LATENCY_MS`: 40 ms FFT latency window
* `int OUTPUT_FFT_MAX_HZ`: 2000 Hz maximum frequency to extract
* `float DELTA_HZ`: 1000/WINDOW_LATENCY_MS: 25 Hz FFT bin width
* `int WINDOW_SIZE`: SAMPLE_RATE * WINDOW_LATENCY_MS / 1000: 1760 samples in an FFT latency window
* `int OUTPUT_FFT_SIZE`: OUTPUT_FFT_MAX_HZ / DELTA_HZ: 80 FFT bins

The other relevant constant defined in this file is the default socket port:

* `int PORTNO`: 10520

A `Shared_Array` (see below) type alias is defined here as well:

    using shared_fft_t = Shared_Array<float,OUTPUT_FFT_SIZE>;

## Test Programs

### Channelbroker

# Other Modules

## include Directory

Miscellaneous functionality encapsulated in headers without associated translation units.

### poller.h

Introduces a Poller class which uses RAII to poll in a loop. At construction, it takes the current time, then adds a specified number of milliseconds to get the time for the next loop iteration. At destruction, sleeps until the specified time.

For example, if you want a loop to iterate every 50 milliseconds, use something like:

    while(!done) {
        Poller poller(50);

        //Loop code here
    }

### shared_array.h

A class template for a shared array, with access locked. The underlying data is a std::array. Supports mutex locking and sequence locking. The internal array cannot be accessed directly; instead, this is used to copy a local buffer to the shared array, or receive a copy of the internal array back as a local buffer.

Writes to the array both lock the mutex and increment the lock sequence appropriately. Write using either of:

    shared_array.write(std::array input)
    shared_array.write(T array[N] input)

Reads from the array return a copy of the std::array. For mutex-locked reading (more efficient) use:

    shared_array.read()

For sequence-based locking, which continually retries the read until the read completes with no intervening write, use:

    shared_array.read_sequence()

### shared_memory.h

A wrapper class for POSIX shared memory. Not in use anymore.

## FFT

Performs FFT according to Oren's approach.

## localcontroller

Performs FFT using the fftw3 library.

Initializes using defaults for `WINDOW_SIZE`, `OUTPUT_FFT_SIZE`, and `SAMPLE_RATE` values, but a constructor is provided to override these values.

Uses a PortAudio callback function, `callback`, to automatically poll and perform FFT.

`GetRefreshRate()` returns the polling rate.

`GetData()` has two dignatures:
* `GetData(double *fft_data_out)` copies FFT bucket count data to the passed array
* `GetData(double *fft_data_out, float *raw_audio_out)` additionally copies raw audio data


## fft2midi

Extracts notes from an FFT array, converts them to MIDI values, then sends the MIDI notes to a MIDI channel on a given port.

### Desynthesizer

Class declared/defined in fft2midi.h/fft2midi.cpp

A wrapper for all fft2midi functionality.

Constructor: `Desynthesizer(int port, unsigned int channel, bool _all, bool _hillclimb)`
* `int port`: the MIDI port to connect to (indexes MIDI software on the system)
* `int channel`: the MIDI channel to connect to (indexes MIDI instrument channels in the software)
* `bool _all`: if true, plays all pitches found, otherwise just the fundamental frequency
* `bool _hillclimb`: if true, uses `tone.Hillclimb()` to extract frequencies, otherwise uses `tone.ExtractSignatures()`

`run()` can be called in a loop to continually extract notes and send them as MIDI

`fft_data()` returns a reference to the tone's underlying interval array. This array can then be populated with the FFT data received over the socket or from the FFT module

### Tone

Performs the note extraction from the FFT data.

`interval` is a `Shared_Array` to hold FFT data.

### MidiStream

A class to send a stream of MIDI notes.

The constructor `MidiStream(int port)` connects to the specified port (defaulting to 0).

### ChannelBroker

There are 16 MIDI channels, corresponding to different instruments. A `ChannelBroker` object wraps these channels, allowing concurrent requests to obtain channels, which are wrapped in a `Channel` class. It assigns channels 0-15 in order, though a request can be made to obtain the drum channel, in which case it assigns channel 9, if free. If not free, it assigns the next available channel. The `Channel` destructor frees the associated channel in the `ChannelBroker`.

The `ChannelBroker` has a default constructor.

The `Channel` constructor is:

`Channel(ChannelBroker & _broker, bool drum)`, taking a reference to a ChannelBroker, and specifying the request for the drum channel.

## Socket

## Tensorflow build

The tensorflow_cc api must be installed to use the AI version of signature extraction. Perform the following:

    # Install prereqs and bazel
    sudo apt-get install cmake curl g++-7 git python3-dev python3-numpy sudo wget
    curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor > bazel.gpg
    sudo mv bazel.gpg /etc/apt/trusted.gpg.d/
    echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | sudo tee /etc/apt/sources.list.d/bazel.list
    sudo apt-get update && sudo apt-get install bazel-3.1.0

    # Clone a helpful repo from FloopCZ to install the API
    git clone https://github.com/FloopCZ/tensorflow_cc.git
    cd tensorflow_cc/tensorflow_cc
    mkdir build && cd build
    cmake -DALLOW_CUDA=off ..
    make
    sudo make install

It can now be linked into CMake projects with `find_package(TensorflowCC REQUIRED)` and `target_link_libraries(example TensorflowCC::TensorflowCC)`