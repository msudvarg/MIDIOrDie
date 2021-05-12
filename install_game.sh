# Clone custom Godot Game engine
git clone https://github.com/dane-johnson/godot
cd godot
git checkout origin/rtmidi

# Install pre-reqs
sudo apt-get update
sudo apt-get install -y build-essential scons pkg-config libx11-dev libxcursor-dev libxinerama-dev \
    libgl1-mesa-dev libglu-dev libasound2-dev libpulse-dev libudev-dev libxi-dev libxrandr-dev yasm
scons -j8 platform=x11

# Change to this repo
cd ../midiordiethegame
git submodule update --init --recursive

# Build bindings for our custom game engine
cd MidiCPP/godot-cpp
scons generate_bindings=yes -j$(nproc)

# Build the game
cd ..
mkdir bin
make

# Use this command to open the Godot-editor, then select "Import project", 
# navigate to the midiordiethegame folder and select the project.godot file.
cd path/to/danes/godot/bin
./godot.x11.tools.64