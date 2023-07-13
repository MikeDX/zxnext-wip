cmake_minimum_required(VERSION 3.0)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/next.cmake)

project(YourProjectName)

# Override object file extension
set(CMAKE_C_OUTPUT_EXTENSION ".o")
set(CMAKE_CXX_OUTPUT_EXTENSION ".o")


# Override Clang-related variables
set(CMAKE_C_COMPILER_TARGET "")
set(CMAKE_OSX_ARCHITECTURES "")
set(CMAKE_OSX_SYSROOT "")

# Set the source files
# file(GLOB_RECURSE SOURCES src/*.c)
# Set the source files in the desired order
set(SOURCES
    src/main.c
    src/layer2.c
    src/sprites.c
)

# Add include directories
include_directories(include)
set(LIBRARY_INCLUDE_PATH "/Users/mike/src/zxnext_layer2/lib/sccz80/")


# Set the compiler flags
set(COMPILER_FLAGS ${CMAKE_C_FLAGS} +zxn -v --list -SO3 -c -clib=new  -pragma-include:zpragma.inc )

# Set the linker flags globally
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "+zxn -vn -startup=1 -m -clib=new -subtype=nex -Cz\"--clean\" -create-app")

# Set the output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)


# Set the desired object file extension
set_source_files_properties(${SOURCES} PROPERTIES SUFFIX ".o")


# Generate the executable with "nex" extension
add_executable(zxn_sprite_1 ${SOURCES})
set_target_properties(zxn_sprite_1 PROPERTIES SUFFIX ".nex")

# Set the compiler flags for the executable
target_compile_options(zxn_sprite_1 PRIVATE ${COMPILER_FLAGS})

# Pass the include library path with -L flag
target_link_libraries(zxn_sprite_1 PRIVATE "-L${LIBRARY_INCLUDE_PATH}")

# Set the final .nex file destination
set(NEX_FILE_DESTINATION ${CMAKE_BINARY_DIR}/bin/zxn_sprite_1.nex)

# Copy the final .nex file to the bin directory
add_custom_command(TARGET zxn_sprite_1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:zxn_sprite_1> ${NEX_FILE_DESTINATION}
    COMMENT "Copying zxn_sprite_1.nex to bin directory"
)