# CMake Version Requirements
cmake_minimum_required (VERSION 3.1)

# Project Name
project (PlanetRenderer)
set (PlanetRenderer_VERSION_MAJOR 1)
set (PlanetRenderer_VERSION_MINOR 0)

# Set directory variables
set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(PROJECT_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# Update the moudule path to find CEGUI
set(CMAKE_MODULE_PATH "./cmake/modules/;${CMAKE_MODULE_PATH}")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set the the output path
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

# Executable PlanetRenderer
add_executable(PlanetRenderer src/gui/MainWindow.h src/gui/MainWindow.moc src/main.cpp)

# Package OpenGL
find_package (OpenGL REQUIRED)
# Use OpenGL Library
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries (PlanetRenderer ${OPENGL_gl_LIBRARIES})
# Use GLU Library
target_link_libraries (PlanetRenderer ${OPENGL_glu_LIBRARIES})

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
add_definitions(${Qt5Widgets_DEFINITIONS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
target_link_libraries (PlanetRenderer ${Qt5Widgets_LIBRARIES})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

# Find the QtDeclarative library
find_package(Qt5Declarative REQUIRED)
add_definitions(${Qt5Declarative_DEFINITIONS})
include_directories(${Qt5Declarative_INCLUDE_DIRS})
target_link_libraries (PlanetRenderer ${Qt5Declarative_LIBRARIES})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Declarative_EXECUTABLE_COMPILE_FLAGS}")

# Find the QtQml library
find_package(Qt5Qml REQUIRED)
add_definitions(${Qt5Qml_DEFINITIONS})
include_directories(${Qt5Qml_INCLUDE_DIRS})
target_link_libraries (PlanetRenderer ${Qt5Qml_LIBRARIES})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Qml_EXECUTABLE_COMPILE_FLAGS}")

# Find the QtQuick library
find_package(Qt5Quick REQUIRED)
add_definitions(${Qt5Quick_DEFINITIONS})
include_directories(${Qt5Quick_INCLUDE_DIRS})
target_link_libraries (PlanetRenderer ${Qt5Quick_LIBRARIES})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Quick_EXECUTABLE_COMPILE_FLAGS}")

# qt5_generate_moc(src/main.cpp src/main.moc)
qt5_generate_moc(src/gui/MainWindow.h src/gui/MainWindow.moc)
