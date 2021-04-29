
git submodule init
git submodule update

#Install dependencies on Debian-based system (e.g. Raspberry Pi)
sudo apt-get update
sudo apt-get install build-essential cmake libfftw3-dev librtmidi-dev portaudio19-dev

mkdir build
cd build
cmake .. -DBUILD_SERVER=ON
make
