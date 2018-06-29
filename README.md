# morphologica

Library code used in models developed by Stuart P. Wilson and co-workers

This code builds a shared library called libmorphologica.

It installs the library on your system, along with the required header
files.

Code is (or shortly will be) enclosed in the "morph" namespace.

It requires OpenCV, Armadillo, OpenGL and X headers to compile, and
programs linked with libmorphologica will also need to link to those
dependencies. You will also need the cmake program and a C++ compiler.

To install these dependencies on Ubuntu or Debian Linux, you can do:

```sh
sudo apt install build-essential cmake libopencv-dev libarmadillo-dev \
                 freeglut3-dev libglu1-mesa-dev libxmu-dev libxi-dev
```

To build, it's the usual CMake process:

```sh
cd morphologica
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make -j4
ctest
sudo make install
sudo ldconfig # Probably Linux specific! Mac alternative?
```

Note the call to ldconfig at the end there, which makes sure that
libmorphologica is available to your system's dynamic linker. On
Linux, that means running ldconfig (assuming that the
CMAKE\_INSTALL\_PREFIX of /usr/local is already in your dynamic
linker's search path) as in the example above. If you installed
elsewhere, then you probably know how to set LD\_CONFIG\_PATH
correctly (or at least you can now search up how to do that).

Note also that we've had a peculiar linking issue with libarmadillo8
on Ubuntu 18.04 LTS. So far the only way I found to solve this was to
install a from-source compiled version of libarmadillo version 8.600
in /usr/local/lib. If you have to do this, then you can pass
-DMORPH_ARMADILLO_LIBPATH=/usr/local/lib and the linker will add this
before -larmadillo to link the one that works, rather than the package
managed one that seems not to (but is required by the OpenCV libs).
