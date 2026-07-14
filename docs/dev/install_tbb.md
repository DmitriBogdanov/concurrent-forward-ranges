## Install TBB from source

Summary of the [official installation guide](https://github.com/uxlfoundation/oneTBB/blob/master/INSTALL.md) for Linux:

**Prerequisites:** [CMake 3.5+](https://cmake.org/)

```bash
# Clone TBB sources
git clone https://github.com/uxlfoundation/oneTBB.git
cd oneTBB

# Build
mkdir build
cd    build

cmake -DTBB_TEST=OFF ..
cmake --build .

# Install
sudo make install
```