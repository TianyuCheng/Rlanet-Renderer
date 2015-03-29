# CMake Version Requirements
cmake_minimum_required (VERSION 3.0)

# Project Name
project (PlanetRenderer)
set (PlanetRenderer_VERSION_MAJOR 1)
set (PlanetRenderer_VERSION_MINOR 0)

# set(CMAKE_DISABLE_SOURCE_CHANGES ON)
# set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

message(${CMAKE_BINARY_DIR})

# Set directory variables
# set(PROJECT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
# set(PROJECT_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# Update the moudule path to find CEGUI
set(CMAKE_MODULE_PATH "cmake/modules/;${CMAKE_MODULE_PATH}")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Auto-moc for Qt
set(CMAKE_AUTOMOC ON)

# Package OpenGL
find_package (OpenGL REQUIRED)
# Use OpenGL Library
include_directories(${OPENGL_INCLUDE_DIR})
# Include our own directories
include_directories(src/)
include_directories(src/ui/)
include_directories(src/utils/)
# include_directories("${PROJECT_SOURCE_DIR}/ui")
# include_directories("${PROJECT_SOURCE_DIR}/planet")

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
add_definitions(${Qt5Widgets_DEFINITIONS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "--std=c++11 -ggdb3 ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

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

# Manually create UI because of the version error in UI form
qt5_wrap_ui(UIS_HDRS ${UI_FORMS})

add_library(PlanetGUI ${UI_HDRS} ${UI_SRCS} ${UIS_HDRS})
set_target_properties(PlanetGUI PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(PlanetGUI ${QT_LIBRARIES})

file(GLOB PLANET_HDRS
    src/utils/*.h
)
file(GLOB PLANET_SRCS
    src/utils/*.cpp
)
add_library(PlanetEssential ${PLANET_HDRS} ${PLANET_SRCS})
set_target_properties(PlanetEssential PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(PlanetEssential ${QT_LIBRARIES})

# Link executable with modules
target_include_directories(PlanetGUI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

AUX_SOURCE_DIRECTORY(src/ SRC)
# Executable PlanetRenderer
add_executable(PlanetRenderer ${SRC})
qt5_use_modules(PlanetRenderer Core Gui Widgets OpenGL)

target_link_libraries(PlanetRenderer PlanetGUI PlanetEssential ${OPENGL_LIBRARIES} ${OPENGL_glu_LIBRARIES})

set(ALL_SRC ${SRC})
string(REPLACE ";" "\n" ALL_SRC "${ALL_SRC}")
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/cscope.files "${ALL_SRC}")
