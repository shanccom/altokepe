<<<<<<< HEAD
cmake_minimum_required(VERSION 3.14)
project(ClienteRanking LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

# Incluir el directorio común del proyecto
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../..)

add_executable(ClienteRanking main.cpp
                 network/RankingClient.cpp network/RankingClient.h
                 ui/RankingWindow.cpp ui/RankingWindow.h)

target_link_libraries(ClienteRanking PRIVATE Qt6::Widgets Qt6::Network)
=======
cmake_minimum_required(VERSION 3.14)
project(ClienteRanking LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

add_executable(
  ClienteRanking main.cpp resources.qrc 
                 network/RankingClient.cpp network/RankingClient.h
                 ui/RankingWindow.cpp ui/RankingWindow.h
                 repository/JsonMenuRepository.cpp repository/JsonMenuRepository.h repository/MenuRepository.h)

target_link_libraries(ClienteRanking PRIVATE Qt6::Widgets Qt6::Network)
>>>>>>> dev-Dolly
