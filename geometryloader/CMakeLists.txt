#cmake_minimum_required(VERSION 3.0)
#project(libgeometryloader VERSION 0.1.0 LANGUAGES CXX)

find_package(Qt5 CONFIG REQUIRED Core Gui)
find_package(GDAL REQUIRED)

set(SRC
	src/GdalLoader.cpp
	src/GdalLoaderFactory.cpp
	src/GeometryLoader.cpp
	src/GeometryLoaderFactory.cpp
)

add_library(geometryloader ${SRC})

target_include_directories(geometryloader
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${GDAL_INCLUDE_DIRS}
)

add_dependencies(geometryloader util)
target_compile_options(geometryloader PRIVATE -fPIC -Wall -Wextra)
target_compile_features(geometryloader PRIVATE cxx_std_17)
target_link_libraries(geometryloader PRIVATE util ${GDAL_LIBRARY} Qt5::Core)
