cmake_minimum_required(VERSION 3.16)
project(ecu_platform)

set(CMAKE_CXX_STANDARD 17)

add_executable(ecu_platform
    src/main.cpp
    src/transport/Transport_CAN_Linux.cpp
    src/core/CAN_Dispatcher.cpp
    src/isotp/ISOTP.cpp
)

target_include_directories(ecu_platform PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/transport
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/isotp
)
