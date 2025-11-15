cmake_minimum_required(VERSION 3.25)
project(eecs300-display VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets SerialPort)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(eecs300-display
    main.cpp
    mainwindow.cpp
    settingsdialog.cpp
    settingsdialog.ui
    console.cpp
)

target_link_libraries(eecs300-display PRIVATE Qt5::Widgets Qt5::SerialPort)

file(COPY ${CMAKE_SOURCE_DIR}/images
     DESTINATION ${CMAKE_BINARY_DIR})

