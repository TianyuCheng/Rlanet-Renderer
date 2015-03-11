# CMake Version Requirements
cmake_minimum_required (VERSION 3.1)

# Project Name
project (PlanetRenderer)
set (PlanetRenderer_VERSION_MAJOR 1)
set (PlanetRenderer_VERSION_MINOR 0)

# Update the moudule path to find CEGUI
set(CMAKE_MODULE_PATH "./cmake/modules/;${CMAKE_MODULE_PATH}")

# Executable PlanetRenderer
add_executable(PlanetRenderer src/main.cpp)

# Required Packages
# -----------------
# Package OpenGL
find_package (OpenGL REQUIRED)

if (OPENGL_FOUND)
    include_directories(${OPENGL_INCLUDE_DIR})
    target_link_libraries (PlanetRenderer ${OPENGL_gl_LIBRARIES})
endif (OPENGL_FOUND)

if (OPENGL_GLU_FOUND)
    target_link_libraries (PlanetRenderer ${OPENGL_glu_LIBRARIES})
endif (OPENGL_GLU_FOUND)

find_package (Qt REQUIRED)
if (QT_FOUND)
    include_directories(${OPENGL_INCLUDE_DIR})
    target_link_libraries (PlanetRenderer ${OPENGL_gl_LIBRARIES})
endif (QT_FOUND)

# # Package GLFW
# find_package (GLFW REQUIRED)
# if (GLFW_FOUND)
#     include_directories(${GLFW_INCLUDE_DIRS})
#     target_link_libraries (PlanetRenderer ${GLFW_LIBRARIES})
# endif (CEGUI_FOUND)
#
# # Package CEGUI
# find_package (CEGUI REQUIRED)
# if (CEGUI_FOUND)
#     include_directories(${CEGUI_INCLUDE_DIRS})
#     target_link_libraries (PlanetRenderer ${CEGUI_LIBRARIES})
# endif (CEGUI_FOUND)
