cmake_minimum_required(VERSION 3.4)


set(CMAKE_PREFIX_PATH "/home/moose/build/Qt/5.12.3/gcc_64/lib/cmake;${CMAKE_PREFIX_PATH}")

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to create code from Qt designer ui files
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)

add_definitions(-fPIC -std=c++17 -Wall -Wextra)

project(citysim VERSION 0.1.0 LANGUAGES CXX)


set(TEST_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test")
set(TEST_BASE_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/test/catch.hpp
)

include(CTest)

# Find the QtWidgets library
find_package(Qt5 CONFIG REQUIRED Core Gui Widgets 3DCore 3DExtras 3DRender 3DInput)

find_package(GDAL REQUIRED)

find_package(Boost REQUIRED COMPONENTS graph)

add_subdirectory(util)
add_subdirectory(core)
add_subdirectory(quadtree)
add_subdirectory(geometryloader)
add_subdirectory(graph)
add_subdirectory(app)

# testing!


#add_subdirectory(test)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
     --force-new-ctest-process
     --verbose
     --output-on-failure ..
)
