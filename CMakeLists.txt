cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(PROJECT_NAME UVLamp)
project(${PROJECT_NAME})


set(CMAKE_CXX_STANDARD 11)

set(${CMAKE_PROJECT_NAME}_BOARD leonardo)
set(${CMAKE_PROJECT_NAME}_PORT COM4)
set(${CMAKE_PROJECT_NAME}_AFLAGS -v)
#set(OPTIMIZE_FLAGS "-Os" CACHE STRING "Optimization flags")  # Remember to reset cache and rebuild cmake when changing this.

set(${CMAKE_PROJECT_NAME}_SKETCH UVLamp.ino)
set(${CMAKE_PROJECT_NAME}_SKETCH Lcd16x2.cpp)

generate_arduino_firmware(${CMAKE_PROJECT_NAME})
