cmake_minimum_required(VERSION 3.16)

project(ServidorAltokepe VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

get_filename_component(PROJECT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
include_directories("${PROJECT_ROOT_DIR}")

find_package(Qt6 REQUIRED COMPONENTS Core Network)

set(MENU_JSON_PATH "${PROJECT_ROOT_DIR}/data/menu.json")
if(NOT EXISTS "${MENU_JSON_PATH}")
  message(FATAL_ERROR "El archivo 'menu.json' no se encuentra en: ${MENU_JSON_PATH}")
endif()

set(COMMON_SERIALIZER_PATH "${PROJECT_ROOT_DIR}/common/network/SerializadorJSON.cpp")
if(NOT EXISTS "${COMMON_SERIALIZER_PATH}")
  message(FATAL_ERROR "No se encontró SerializadorJSON.cpp en: ${COMMON_SERIALIZER_PATH}")
endif()

add_executable(ServidorAltokepe
  main.cpp
  Servidor.cpp
  ManejadorCliente.cpp
  LogicaNegocio.cpp
  patterns/ServerCommands.cpp
  patterns/CommandFactory.cpp
  identifier/IdentificadorCliente.cpp
  resources.qrc
  ${COMMON_SERIALIZER_PATH}
)

target_link_libraries(ServidorAltokepe PRIVATE
  Qt6::Core
  Qt6::Network
)

add_custom_command(
  TARGET ServidorAltokepe POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
          "${MENU_JSON_PATH}"
          "$<TARGET_FILE_DIR:ServidorAltokepe>/"
  COMMENT "Copiando menu.json desde data/ al directorio de compilación"
)
