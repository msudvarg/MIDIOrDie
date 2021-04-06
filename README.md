# Compiliation
1. Initialize the git submodules (`git submodule init; git submodule update`)
2. Install fftw (the only dependency, all others are built from source)
3. Create a build directory beneath this one (`mkdir build`)
4. Change to the build directory, initialize cmake, and build (`cd build; cmake ..; cmake --build`)

# Command Line Tools
## Controller
The controller collects audio data from the audio input and performs the fast Fourier transform. It searches for a server socket, and send the data over the wire.

Its options are as follows
- i \<ip-addr\>: Set the ip address to look for a host under
- f: Run forever. Omitting this flag will run for 1000 iterations

## Server
The server is a proxy for the host application. It acts as a server and channel broker for the controllers. It performs the translation from the fourier series sent from the controller to Midi notes, and sends the midi notes to the host application on the designated port, one Midi channel per instrument.

Its options are as follows
- p \<port-num\>: This designates the midi port on which to send the notes. Defaults to 0, the hardware midi port. If you are unsure of which port to use, the server will print all known ports and the names of the application using them when it initializes
- a: Print all midi notes detected. If absent the program will only send the Midi note with the lowest frequency
- d: If this option is present, the first controller will be mapped to Midi channel 10, which is the drum channel. If absent, the broker will begin mapping channels at 1.
- c: This activates hillclimbing mode, which detects local maxima on the Fourier series and translates them to midi notes. If it is absent, the server will use the signature recognition.
