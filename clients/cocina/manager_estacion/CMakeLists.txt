cmake_minimum_required(VERSION 3.16)

project(ManagerChefClient VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

# Añadir el directorio raíz ('altokepe/') a las rutas de inclusión para el compilador.
include_directories(../../../)

# Definir el ejecutable y sus archivos fuente.
add_executable(ManagerChefClient
  main.cpp
  
  # Capa de Red
  network/ClienteEstacionApp.cpp
  network/ClienteTCP.cpp
)

# Vincular las librerías de Qt al ejecutable
target_link_libraries(ManagerChefClient PRIVATE
  Qt6::Widgets
  Qt6::Network
)
