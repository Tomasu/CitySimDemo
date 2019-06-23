#cmake_minimum_required(VERSION 3.0)
#project(libgraph VERSION 0.1.0 LANGUAGES CXX)

find_package(Boost REQUIRED COMPONENTS graph)

set(SRC
	src/EdgeProperties.cpp
	src/GraphNode.cpp
	src/TransportGraph.cpp
	src/VertexProperties.cpp
)

add_library(graph ${SRC})

target_include_directories(graph
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_dependencies(graph util)
target_link_libraries(graph PRIVATE util Boost::graph)
target_compile_options(graph PRIVATE -fPIC -Wall -Wextra)
target_compile_features(graph PRIVATE cxx_std_17)
