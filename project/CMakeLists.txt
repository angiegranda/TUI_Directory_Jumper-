cmake_minimum_required(VERSION 3.29)

set(CMAKE_CXX_STANDARD 20)

project(
    tcd
    VERSION 0.0.1
    LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
    src/controller/Buffer.cpp
    src/controller/Controller.cpp
    src/controller/Explorer.cpp
    src/ui/UI.cpp
    src/ui/TerminalWriter.cpp 
    src/controller/QuickAccess.cpp
    src/ui/QuickAccessView.cpp
    src/ui/NavegationView.cpp
    src/Main.cpp
)

if(WIN32)
    list(APPEND SOURCES src/ui/WindowsResizeMonitor.cpp)
endif()

add_executable(tcds ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/controller)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/ui)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
