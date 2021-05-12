bash install_base.sh

sudo apt-get install -y librtmidi-dev

bash install_tensorflow.sh

cd build
cmake .. -DBUILD_SERVER=ON
make
