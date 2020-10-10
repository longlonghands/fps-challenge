# fps-challenge
a test using libav to calculate fps from pts

To build the code you need: GCC 8.0 or above, cmake 3.2 or above, libav*-dev 4.0 or above.

On linux, You can install them using linux package managers.

On windows its better to install mingw using Stephan Lavavej's distro (https://nuwen.net/mingw.html). I used Zeranoe's build for my test. but this build is not available anymore. The next best thing is to use mingw and build libav for yourself using [https://github.com/m-ab-s/media-autobuild_suite].

To compile the code simply enter these commands

`mkdir build`

`cd build`

`cmake ..`

`make -j 4`


The program name is arvan-challenge and it will be in build directory.

`
./arvan-challenge [input url]
`
