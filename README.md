# morphologica

Library code used in models developed by Stuart P. Wilson and co-workers

This code builds a shared library called libmorphologica.

It installs the library on your system, along with the required header
files.

Code is (or shortly will be) enclosed in the "morph" namespace.

It requires OpenCV, Armadillo and OpenGL headers to compile, and
programs linked with libmorphologica will also need to link to those
dependencies. You will also need the cmake program and a C++ compiler.

To install these dependencies on Ubuntu or Debian Linux, you can do:

```
sudo apt install build-essential cmake libopencv-dev libarmadillo-dev libglu1-mesa-dev
```

To build, it's the usual CMake process:

```
cd morphologica
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make -j4
ctest
sudo make install
```
