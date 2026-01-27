# LBR

Welcome to our CMake build system for Embedded Software!

## Installing Prerequisites
First, install the arm toolchain. If you are on a Unix system, simply use your package installer, for example in Ubuntu: ```sudo apt-get install arm-none-eabi-gcc```.
On Windows, you can download it from online, try [here](https://developer.arm.com/downloads/-/gnu-rm) - you may have to add it to your path. Make sure it worked: ```arm-none-eabi-gcc --version```

<!--
If you want to cross-compile for 32-bit RPI, also install the arm32 toolchain: ```gcc-arm-linux-gnueabihf```.
On Windows, try [here](https://developer.arm.com/downloads/-/gnu-a). Make sure it worked: ```arm-linux-gnueabihf-gcc --version```
-->

Next, install Cmake. ```sudo apt-get install cmake```
On Windows, try [here](https://cmake.org/download/). Again, may have to add to path.
Make sure it worked: ```cmake --version```

Also, install Ninja ```sudo apt-get install ninja-build```
On Windows, try [here](https://github.com/ninja-build/ninja/releases). Same with adding to path.
Make sure it worked: ```ninja --version```

Lastly, pull in external dependencies:
```git submodule update --init --recursive```

## Building
To build:
If you are on Windows, use the ```.ps1``` script in powershell. If on Linux / Unix, use the ```.sh``` script. The minimum parameters look like this: 
```./make.ps1 -t <name of preset>```.
For example, ```./make.ps1 -t stm32f746``` (see CMakePresets.json).

It's also possible to specify a target application rather than building all available apps (which is the default), by using the -a parameter: ```./make.ps1 -t stm32f746 -a blink```. 

for a clean build, do
```./make.ps1 -t <name of preset> -c```

builds are by default done in Debug mode, but Release mode can be selected with the -r parameter: ```./make.ps1 -t stm32f746 -r```

## Running tests
Build for native with:
```make.sh -t native``` or the ```.ps1``` if on windows. 
After navigate to the build directory with ```pushd build/native``` followed by ```ctest``` to run tests! You can now to back with ```pushd```.

## Debugging
To debug, make sure you have openocd installed ```sudo apt-get install openocd```
On Windows, try [here](https://openocd.org/pages/getting-openocd.html). Also may have to add to path.
Additionally, grab the cortex-debug extension for VSCode.
There are reference launch.json files found in the repository already under .vscode.

## Developing
Install clang-format to auto-format your code - on Windows, try ```<python> -m pip install clang-format```. On Linux, try ```sudo apt install clang-format```. In VSCode, you can go to settings > Text Editor > Formatting > Format On Save to enable auto-formatting on save.

<!--
To run native unit tests, you can open a PR on Github or build for native, then run ```pushd build/native; ctest; popd```
-->
