
git submodule init
git submodule update

#Install dependencies on Debian-based system (e.g. Raspberry Pi)
sudo apt-get install cmake jack libfftw3-dev librtmidi-dev portaudio19-dev

mkdir build
cd build
cmake ..
