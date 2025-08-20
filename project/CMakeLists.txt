cmake_minimum_required(VERSION 3.29)

set(CMAKE_CXX_STANDARD 20)

project(
    cfd
    VERSION 0.0.1
    LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
    src/Buffer.cpp
    src/Controller.cpp
    src/Explorer.cpp
    src/UI.cpp

    src/Main.cpp
)

if(WIN32)
    list(APPEND SOURCES src/WindowsResizeMonitor.cpp)
endif()

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
