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
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(LIBRARY_INCLUDE_PATH "/Users/mike/src/zxnext_layer2/lib/sccz80/")

set(CLIB "-clib=new")
# set(CLIB -clib=sdcc_iy -SO3 --max-allocs-per-node200000)


# Set the compiler flags
set(COMPILER_FLAGS ${CMAKE_C_FLAGS} +zxn -vn --list -c ${CLIB} -s -pragma-include:${CMAKE_CURRENT_SOURCE_DIR}/zpragma.inc )


# Set the linker flags globally
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "+zxn -vn -startup=1 -m ${CLIB} -pragma-include:${CMAKE_CURRENT_SOURCE_DIR}/zpragma.inc -subtype=nex -Cz\"--clean\" -create-app")

set(CLIB "-clib=new")
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

# Copy additional files to the bin directory
set(ASSETS_DESTINATION ${CMAKE_BINARY_DIR}/bin)
set(BUILD_DESTINATION ${CMAKE_BINARY_DIR}/build)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/all.nxp ${CMAKE_CURRENT_SOURCE_DIR}/assets/all.spr ${CMAKE_CURRENT_SOURCE_DIR}/assets/screen1.nxi DESTINATION ${ASSETS_DESTINATION})

# Set the final .nex file destination
set(NEX_FILE_DESTINATION ${CMAKE_BINARY_DIR}/bin/zxn_sprite_1.nex)

# Copy the final .nex file to the bin directory
add_custom_command(TARGET zxn_sprite_1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:zxn_sprite_1> ${NEX_FILE_DESTINATION}
#     COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/assets/all.nxp ${ASSETS_DESTINATION}
#     COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/assets/all.spr ${ASSETS_DESTINATION}
#     COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/assets/screen1.nxi ${ASSETS_DESTINATION}
    COMMENT "Copying zxn_sprite_1.nex to bin directory"
)


# Create an "install" target to copy files during installation
install(TARGETS zxn_sprite_1
    RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

# Copy additional files during installation
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/assets/all.nxp ${CMAKE_CURRENT_SOURCE_DIR}/assets/all.spr ${CMAKE_CURRENT_SOURCE_DIR}/assets/screen1.nxi
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

# Set the additional files and .nex file as output
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${ASSETS_DESTINATION}/all.nxp;${ASSETS_DESTINATION}/all.spr;${ASSETS_DESTINATION}/screen1.nxi;${NEX_FILE_DESTINATION};${BUILD_DESTINATION}/zxn_sprite_1.map")