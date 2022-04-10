# CMakeList.txt : CMake project for chesswrap, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (chesswrap "main.cpp"  "pieces.h" "global.h" "game_state.h" "game_state.cpp" "pieces.cpp" "global.cpp")

target_link_libraries(chesswrap SDL2::SDL2 SDL2::SDL2main ${SDL_LIBRARIES})

if(MINGW)
target_link_libraries(chesswrap mingw32)
endif()

if(MSVC)
target_link_options(chesswrap PRIVATE /SUBSYSTEM:WINDOWS)
message("${LINK_OPTIONS}")
endif()

set(CHESSWRAP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(CHESSWRAP_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)

# TODO: Add tests and install targets if needed.
