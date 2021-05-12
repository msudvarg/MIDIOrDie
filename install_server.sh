bash install_base.sh

sudo apt-get install -y librtmidi-dev

mkdir build
cd build

bash install_tensorflow.sh

cmake .. -DBUILD_SERVER=ON
make
