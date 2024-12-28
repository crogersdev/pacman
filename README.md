## pacman
libsfml c++ implementation of pacman

### build instructions
builds on ubuntu.  here's a list of potentially overlapping but necessary dependencies:

`$ sudo apt install cmake build-essential`

download and install libsfml 3.0.0 from source<br />
at the time of writing, the `apt` source of libsfml, `libsfml-dev`, installs version 2.6</br>

`cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DSFML_BUILD_EXAMPLES=True -DSFML_INSTALL_PKGCONFIG_FILES=True ..`

for good measure do autoconf and libtool because building C and C++ projects on linux often requires them:

`$ sudo apt install libtool autoconf`

install VSCode extensisons for CMake and C++ (TODO: add links)

in VSCode, ctrl+shift+p; then find CMake build commands (TODO: polish this instruction)

TODO: provide command line invocation of g++ with compiler and linker options so using CMake extension isn't required
