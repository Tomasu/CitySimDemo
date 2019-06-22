project(quadtree CXX)

set(CMAKE_CXX_STANDARD 17)
add_definitions(-fPIC -std=c++17 -Wall -Wextra)

set(CMAKE_PREFIX_PATH "/home/moose/build/Qt/5.12.3/gcc_64/lib/cmake;${CMAKE_PREFIX_PATH}" PARENT_SCOPE)

find_package(Qt5 CONFIG REQUIRED Core Gui Widgets 3DCore 3DExtras 3DRender 3DInput)
find_package(GDAL REQUIRED)

list(APPEND INCLIST ${Qt5Core_INCLUDE_DIRS})
list(APPEND INCLIST ${Qt5Gui_INCLUDE_DIRS})
list(APPEND INCLIST ${GDAL_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/src ${INCLIST})



set(SRC
	QuadTree.cpp
	QuadTree.h
	QuadTreeNode.cpp
	QuadTreeNode.h
	QuadTreeNodeItem.cpp
	QuadTreeNodeItem.h
	QuadTreeNodeGeometry.cpp
	QuadTreeNodeGeometry.h
	QuadTreeNodeGeometryGenerator.cpp
	QuadTreeNodeGeometryGenerator.h
	QuadTreeNodeGeometryRenderer.cpp
	QuadTreeNodeGeometryRenderer.h
	QuadTreeNodeGeometryFactory.cpp
	QuadTreeNodeGeometryFactory.h
	QuadTreeNodeEntity.cpp
	QuadTreeNodeEntity.h
)

add_library(quadtree STATIC ${SRC})
target_link_libraries(quadtree Qt5::Core)
