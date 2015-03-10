# CMake Version Requirements
cmake_minimum_required (VERSION 3.1)

# Project Name
project (PlanetRenderer)

# Executable PlanetRenderer
add_executable(PlanetRenderer src/main.cpp)

# Required Packages
# -----------------
# Package OpenGL
find_package (OpenGL)
if (OPENGL_NOT_FOUND)
    MESSAGE(OpenGL is not found! Exit.)
    EXIT()
else (OPENGL_FOUND)
    include_directories(${OPENGL_INCLUDE_DIR})
    target_link_libraries (PlanetRenderer ${OPENGL_gl_LIBRARIES})
endif (OPENGL_NOT_FOUND)

if (OPENGL_GLU_FOUND)
    target_link_libraries (PlanetRenderer ${OPENGL_glu_LIBRARIES})
endif (OPENGL_GLU_FOUND)

# Package CEGUI
find_package (CEGUI)
if (CEGUI_NOT_FOUND)
    MESSAGE(CEGUI is not found! Exit.)
    EXIT()
else (CEGUI_FOUND)
    include_directories(${CEGUI_INCLUDE_DIRS})
    target_link_libraries (PlanetRenderer ${CEGUI_LIBRARIES})
endif (CEGUI_NOT_FOUND)
