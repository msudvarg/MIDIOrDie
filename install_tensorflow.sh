# Install prereqs and bazel
sudo apt-get install -y cmake curl g++-7 git python3 python3-dev python3-numpy sudo wget
mkdir build
cd build
curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor > bazel.gpg
sudo mv bazel.gpg /etc/apt/trusted.gpg.d/
echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | sudo tee /etc/apt/sources.list.d/bazel.list
sudo apt-get update
sudo apt-get install bazel-3.1.0
sudo ln -s /usr/bin/bazel-3.1.0 /usr/bin/bazel

# Clone a helpful repo from FloopCZ to install the API
cd ../
git clone https://github.com/FloopCZ/tensorflow_cc.git
cd tensorflow_cc/tensorflow_cc
mkdir build
cd build
cmake -DALLOW_CUDA=off ..
make
sudo make install

cd ../../../