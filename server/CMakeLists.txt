cmake_minimum_required(VERSION 3.10)
project(ServidorAltokepe LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Network)

add_executable(ServidorAltokepe
    main.cpp
    servidor.h
)

target_link_libraries(ServidorAltokepe
    Qt5::Core
    Qt5::Network
)
