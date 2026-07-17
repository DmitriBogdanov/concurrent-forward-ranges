## Install TBB from source

Summary of the [official installation guide](https://github.com/uxlfoundation/oneTBB/blob/master/INSTALL.md) for Linux:

**Prerequisites:** [CMake 3.5+](https://cmake.org/)

```bash
# Clone
git clone https://github.com/uxlfoundation/oneTBB.git

# Build
cd $oneTBB
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=$HOME/.local/tbb -DTBB_TEST=OFF
cmake --build build

# Install
cmake --install build
```