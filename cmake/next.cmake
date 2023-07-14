set(CMAKE_C_OUTPUT_EXTENSION ".o")
set(CMAKE_CXX_OUTPUT_EXTENSION ".o")

# the name of the target operating system
set(CMAKE_SYSTEM_NAME Linux)

# which compilers to use for C and C++
set(CMAKE_C_COMPILER   zcc)
set(CMAKE_CXX_COMPILER zcc)

# where is the target environment located
set(CMAKE_FIND_ROOT_PATH  /Users/mike/src/stuff/z88dk/)

# search programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


# Override Clang-related variables
set(CMAKE_C_COMPILER_TARGET "")
set(CMAKE_OSX_ARCHITECTURES "")
set(CMAKE_OSX_SYSROOT "")
