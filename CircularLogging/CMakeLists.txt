cmake_minimum_required(VERSION 3.16)

project(CircularLogging LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(CircularLogging
  main.cpp
  logger.h logger.cpp

  CircularLogger.pro
  build/Desktop_Qt_6_9_1_MinGW_64_bit-Debug/config.json



)
target_link_libraries(CircularLogging Qt${QT_VERSION_MAJOR}::Core)

include(GNUInstallDirs)
install(TARGETS CircularLogging
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
