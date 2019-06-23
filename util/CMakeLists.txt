cmake_minimum_required(VERSION 3.0)
project(libutil VERSION 0.1.0 LANGUAGES CXX)

set(SRC
	src/NullPointerException.cpp
	src/Point.cpp
	src/Rect.cpp
	src/Size.cpp
	src/LogUtils.cpp
)

add_library(util STATIC ${SRC})

target_include_directories(util
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(util PRIVATE -fPIC -Wall -Wextra)
target_compile_features(util PRIVATE cxx_std_17)
