# Requirements
### OpenCL-compatible device
For example, a GPU can be checked with `TechPowerUp GPU-Z`. 

On Windows you can install it via `winget`:
```bash
$ winget install TechPowerUp.GPU-Z
```

### Up-to-date drivers
Install latest drivers from your vendor.

AMD devices: https://www.amd.com/en/support

### OpenCL SDK
For Windows, a prepared SDK can be found [here](https://github.com/GPUOpen-LibrariesAndSDKs/OCL-SDK/releases/latest).

### C/C++ Compiler
Clang and GCC should work fine.

MinGW (Windows-compatible GCC) can be downloaded from [here](https://winlibs.com/).

### CMake
```bash
$ cmake --build .
```
Or load this CMake project in your preferred IDE.

