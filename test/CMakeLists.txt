set(CTEST_PROJECT_NAME "CitySim")

set(CMAKE_CXX_STANDARD 17)

list(APPEND INCLIST "${CMAKE_SOURCE_DIR}/src")
list(APPEND INCLIST ${GDAL_INCLUDE_DIRS})
include_directories(${INCLIST})

set(CMAKE_PREFIX_PATH "/home/moose/build/Qt/5.12.3/gcc_64/lib/cmake;${CMAKE_PREFIX_PATH}" PARENT_SCOPE)

find_package(Qt5 CONFIG REQUIRED Core Gui Widgets 3DCore 3DExtras 3DRender 3DInput)
find_package(GDAL REQUIRED)

add_executable(quadtreetest quadtree.cpp catch.hpp)

target_link_libraries(quadtreetest quadtree core util Qt5::Core Qt5::3DExtras Qt5::3DRender ${GDAL_LIBRARY})
add_dependencies(quadtreetest quadtree core util)
add_test(quadtreetest quadtreetest)
