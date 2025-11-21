cmake_minimum_required(VERSION 3.16)

project(ServidorAltokepe VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Habilitar manejo automático de MOC, UIC, y RCC de Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Definir la ruta raíz 'altokepe/' para el proyecto
get_filename_component(PROJECT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
include_directories("${PROJECT_ROOT_DIR}")

find_package(Qt6 REQUIRED COMPONENTS Core Network)

set(COMMON_SERIALIZER_PATH "${PROJECT_ROOT_DIR}/common/network/SerializadorJSON.cpp")
if(NOT EXISTS "${COMMON_SERIALIZER_PATH}")
  message(FATAL_ERROR "No se encontró SerializadorJSON.cpp en: ${COMMON_SERIALIZER_PATH}")
endif()

add_executable(ServidorAltokepe
  main.cpp
  Servidor.cpp
  ManejadorCliente.cpp
  ${COMMON_SERIALIZER_PATH}
)

# Vincular las librerías de Qt al ejecutable
target_link_libraries(ServidorAltokepe PRIVATE
  Qt6::Core
  Qt6::Network
)