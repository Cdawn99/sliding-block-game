project(sliding_block_game)
cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 11)

find_package(raylib 3.0 REQUIRED)

configure_file(save_filepath.c.in ${CMAKE_CURRENT_BINARY_DIR}/save_filepath.c @ONLY)

add_executable(${PROJECT_NAME} main.c entity.c highscore.c ${CMAKE_CURRENT_BINARY_DIR}/save_filepath.c)
target_link_libraries(${PROJECT_NAME} raylib m)
