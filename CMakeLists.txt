# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

if(DEFINED ENV{VCPKG_ROOT})
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()
message("toolchain file: ${CMAKE_TOOLCHAIN_FILE}")

project ("chesswrap")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)
#set(CMAKE_BUILD_TYPE Release)

if (DEFINED ENV{VCPKG_ROOT})
include_directories(
    "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")
link_directories(
    "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib"
    "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib"
)

endif()

find_path(STB_INCLUDE_DIRS "stb.h")
find_package(SDL2 CONFIG REQUIRED)

# Include sub-projects.
add_subdirectory ("src")
add_subdirectory("resources")

add_dependencies(chesswrap CopyAssets)