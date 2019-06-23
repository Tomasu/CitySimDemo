cmake_minimum_required(VERSION 3.0)
project(quadtree VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
add_definitions(-fPIC -std=c++17 -Wall -Wextra)

set(CMAKE_PREFIX_PATH "/home/moose/build/Qt/5.12.3/gcc_64/lib/cmake;${CMAKE_PREFIX_PATH}")

find_package(Qt5 CONFIG REQUIRED Core Gui 3DCore 3DExtras 3DRender)

list(APPEND INCLIST ${Qt5Core_INCLUDE_DIRS})
list(APPEND INCLIST ${Qt5Gui_INCLUDE_DIRS})

set(SRC
	src/QuadTree.cpp
	src/QuadTreeNode.cpp
	src/QuadTreeNodeItem.cpp
	include/quadtree/QuadTreeNodeGeometry.h
	src/QuadTreeNodeGeometry.cpp
	src/QuadTreeNodeGeometryGenerator.cpp
	include/quadtree/QuadTreeNodeGeometryRenderer.h
	src/QuadTreeNodeGeometryRenderer.cpp
	src/QuadTreeNodeGeometryFactory.cpp
	include/quadtree/QuadTreeNodeEntity.h
	src/QuadTreeNodeEntity.cpp
)

add_library(quadtree STATIC ${SRC})
target_include_directories(quadtree
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_dependencies(quadtree util core)
target_compile_options(quadtree PRIVATE -fPIC -Wall -Wextra)
target_compile_features(quadtree PRIVATE cxx_std_17)
target_link_libraries(quadtree util core Qt5::Core Qt5::Gui)
