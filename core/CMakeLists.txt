#cmake_minimum_required(VERSION 3.0)
#project(libcore VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "/home/moose/build/Qt/5.12.3/gcc_64/lib/cmake;${CMAKE_PREFIX_PATH}")
find_package(Qt5 CONFIG REQUIRED Core Gui 3DCore 3DRender)
find_package(GDAL REQUIRED)

set(SRC
	src/BufferBuilder.cpp
	src/GeometryBuilder.cpp
	src/Line.cpp
	src/LineString.cpp
	src/LogUtilsQt.cpp
)

add_library(core STATIC ${SRC})

target_include_directories(core
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

include_directories(${GDAL_INCLUDE_DIRS})
target_compile_options(core PRIVATE -fPIC -Wall -Wextra)
target_compile_features(core PRIVATE cxx_std_17)
target_link_libraries(core PRIVATE util Qt5::Core Qt5::Gui Qt5::3DCore Qt5::3DRender ${GDAL_LIBRARY})
add_dependencies(core util)
