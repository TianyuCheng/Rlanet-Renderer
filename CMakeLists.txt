cmake_minimum_required(VERSION 3.0)

project (PlanetRenderer)
include(ExternalProject)
set (PlanetRenderer_VERSION_MAJOR 1)
set (PlanetRenderer_VERSION_MINOR 0)

message(${CMAKE_BINARY_DIR})

# Update the moudule path to find 3rd party libraries
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/;${CMAKE_MODULE_PATH}")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Auto-moc for Qt
set(CMAKE_AUTOMOC ON)

# Package LibNoise
ExternalProject_Add(project_libnoise
	PREFIX ${CMAKE_CURRENT_BINARY_DIR}/libnoise # install to <build dir>/libnoise
	GIT_SUBMODULES ${CMAKE_SOURCE_DIR}/3rdparty/libnoise/	# update the submodule
	SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/libnoise/
	BINARY_DIR ${CMAKE_BINARY_DIR}/3rdparty/libnoise/	    # build under libnoise/build
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
	BUILD_COMMAND make -j4
	INSTALL_DIR ${CMAKE_BINARY_DIR}
	)
ExternalProject_Get_Property(project_libnoise install_dir)
include_directories(${install_dir}/include)
SET(LIBNOISE_LIBRARY ${install_dir}/lib/libnoise.a)


# Package OpenGL
find_package (OpenGL REQUIRED)
include_directories(src/)
include_directories(src/ui/)
include_directories(src/utils/)
include_directories(src/scene/)

# C++11 Debug
set(CMAKE_CXX_FLAGS "--std=c++11 -ggdb3 ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

find_package(Qt5 COMPONENTS Quick Core OpenGL REQUIRED)
include_directories(${Qt5Quick_INCLUDE_DIRS})

# Enable QtOpenGL module
set(QT_USE_QTOPENGL TRUE)
set(QT_LIBRARIES ${Qt5OpenGL_LIBRARIES})

qt5_add_resources(RESOURCES src/quick/quick.qrc)

file(GLOB PLANET_HDRS
    src/utils/*.h,
    src/scene/*.h
)
AUX_SOURCE_DIRECTORY(src/utils/ PLANET_UTIL_SRCS)
AUX_SOURCE_DIRECTORY(src/scene/ PLANET_SCENE_SRCS)
add_library(PlanetEssential ${PLANET_HDRS} ${PLANET_UTIL_SRCS} ${PLANET_SCENE_SRCS})
set_target_properties(PlanetEssential PROPERTIES LINKER_LANGUAGE CXX)

add_dependencies(PlanetEssential project_libnoise)
target_link_libraries(PlanetEssential ${QT_LIBRARIES} ${LIBNOISE_LIBRARY})

# Executable PlanetRenderer
AUX_SOURCE_DIRECTORY(src/ SRC)
add_executable(PlanetRenderer ${SRC} ${RESOURCES})
qt5_use_modules(PlanetRenderer Quick Core OpenGL)

add_dependencies(PlanetRenderer project_libnoise)
target_link_libraries(PlanetRenderer PlanetEssential ${OPENGL_LIBRARIES} ${OPENGL_glu_LIBRARIES})

# Generate cscope.files
set(ALL_SRC ${SRC} ${PLANET_HDRS} ${PLANET_UTIL_SRCS} ${PLANET_SCENE_SRCS})
string(REPLACE ";" "\n" ALL_SRC "${ALL_SRC}")
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/cscope.files "${ALL_SRC}")
