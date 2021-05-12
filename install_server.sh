bash install_base.sh

sudo apt-get install librtmidi-dev

bash install_tensorflow.sh

mkdir build
cd build
cmake .. -DBUILD_SERVER=ON
make
