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

# Auto-moc for Qt
set(CMAKE_AUTOMOC ON)

# Set the the output path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

# Package OpenGL
find_package (OpenGL REQUIRED)
# Use OpenGL Library
include_directories(${OPENGL_INCLUDE_DIR})

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
add_definitions(${Qt5Widgets_DEFINITIONS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

# Find the QtOpenGL library
find_package(Qt5OpenGL REQUIRED)
add_definitions(${Qt5OpenGL_DEFINITIONS})
include_directories(${Qt5OpenGL_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5OpenGL_EXECUTABLE_COMPILE_FLAGS}")

# Combine all Qt libraries into QT_LIBRARIES
set(QT_USE_QTOPENGL TRUE)
set(QT_LIBRARIES ${Qt5Widgets_LIBRARIES} ${Qt5OpenGL_LIBRARIES})

# Build submodules as libraries
file(GLOB UI_HDRS
    src/ui/*.h
)
file(GLOB UI_SRCS
    src/ui/*.cpp
)
file(GLOB UI_FORMS
    src/ui_forms/*.ui
)
include_directories("${PROJECT_SOURCE_DIR}/ui")
# Manually create UI because of the version error in UI form
qt5_wrap_ui(UIS_HDRS ${UI_FORMS})

add_library(PlanetGUI ${UI_HDRS} ${UI_SRCS} ${UIS_HDRS})
set_target_properties(PlanetGUI PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(PlanetGUI ${QT_LIBRARIES})

# Link executable with modules
target_include_directories(PlanetGUI PUBLIC ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
target_include_directories(PlanetGUI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Executable PlanetRenderer
add_executable(PlanetRenderer src/main.cpp)
qt5_use_modules(PlanetRenderer Core Gui Widgets OpenGL)
target_link_libraries(PlanetRenderer PlanetGUI ${OPENGL_LIBRARIES} ${OPENGL_glu_LIBRARIES})
