sudo apt-get install libasound-dev
cd portaudio
git submodule init
git submodule update
./configure --enable-cxx
make -j4
sudo make install
