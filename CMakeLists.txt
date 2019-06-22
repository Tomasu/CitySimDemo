cmake_minimum_required(VERSION 3.0)
project(citysim)

include(CTest)

set(CMAKE_CXX_STANDARD 17)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to create code from Qt designer ui files
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/home/moose/build/Qt/5.12.3/gcc_64/lib/cmake;${CMAKE_PREFIX_PATH}")

# Find the QtWidgets library
find_package(Qt5 CONFIG REQUIRED Core Gui Widgets 3DCore 3DExtras 3DRender 3DInput)

find_package(GDAL REQUIRED)

find_package(Boost REQUIRED COMPONENTS graph)

add_subdirectory(src/quadtree)

set(citysim_SRC src/main.cpp
	src/MainWindow.cpp
	src/MainWindow.h
	src/ShpMeshGeometry.cpp
	src/ShpMeshGeometry.h
	src/LocUtil.h
	src/GeometryBuilder.cpp
	src/GeometryBuilder.h
	src/Line.cpp
	src/Line.h
	src/LineString.cpp
	src/LineString.h
	src/Plane3D.cpp
	src/Plane3D.h
	src/Constants.h
	src/BufferBuilder.cpp
	src/BufferBuilder.h
	src/ShpMeshRenderer.cpp
	src/ShpMeshRenderer.h
	src/ShpEntity.cpp
	src/ShpEntity.h
	src/graph/GraphNode.cpp
	src/graph/GraphNode.h
	src/graph/TransportGraph.cpp
	src/graph/TransportGraph.h
	src/graph/EdgeProperties.cpp
	src/graph/EdgeProperties.h
	src/graph/VertexProperties.cpp
	src/graph/VertexProperties.h
	src/ShpRoadMapEntity.cpp
	src/ShpRoadMapEntity.h
	src/RoadData.cpp
	src/RoadData.h
	src/aspect/FpsMonitorComponent.cpp
	src/aspect/FpsMonitorComponent.h
	src/aspect/FpsMonitorBackend.cpp
	src/aspect/FpsMonitorBackend.h
	src/aspect/FpsMonitorAspect.cpp
	src/aspect/FpsMonitorAspect.h
	src/aspect/FpsMonitorMapper.cpp
	src/aspect/FpsMonitorMapper.h
	src/aspect/FpsMonitor.cpp
	src/aspect/FpsMonitor.h
	src/aspect/UpdateFpsMonitorsJob.cpp
	src/aspect/UpdateFpsMonitorsJob.h
	src/aspect/MovingAverage.cpp
	src/aspect/MovingAverage.h
	src/LogUtils.cpp
	src/LogUtils.h
	src/RootEntity.cpp
	src/RootEntity.h
	src/util/NullPointerException.cpp
	src/util/NullPointerException.h
	src/util/Preconditions.h
	src/util/Rect.h
	src/util/Rect.cpp
	src/util/Point.h
	src/util/Point.cpp
	src/util/Size.h
	src/util/Size.cpp
	src/geometryloader/GeometryLoader.cpp
	src/geometryloader/GeometryLoader.h
	src/geometryloader/GeometryLoaderFactory.cpp
	src/geometryloader/GeometryLoaderFactory.h
	src/geometryloader/GdalLoader.cpp
	src/geometryloader/GdalLoader.h
	src/geometryloader/GdalLoaderFactory.cpp
	src/geometryloader/GdalLoaderFactory.h
)

message(STATUS "SRC ${citysim_SRC}")

add_definitions(-fPIC -std=c++17 -Wall -Wextra)

list(APPEND INCLIST ${Qt5Core_INCLUDE_DIRS})
list(APPEND INCLIST ${Qt5Widgets_INCLUDE_DIRS})
list(APPEND INCLIST ${Qt5Gui_INCLUDE_DIRS})
list(APPEND INCLIST ${Qt53DCore_INCLUDE_DIRS})
list(APPEND INCLIST ${Qt53DExtras_INCLUDE_DIRS})
list(APPEND INCLIST ${Qt53DRender_INCLUDE_DIRS})
list(APPEND INCLIST ${Qt53DInput_INCLUDE_DIRS})
list(APPEND INCLIST ${GDAL_INCLUDE_DIRS})
list(APPEND INCLIST ${Boost_INCLUDE_DIRS})

#list(JOIN INCLIST " " INCLISTVAL)
#message(STATUS INCLIST ${INCLISTVAL})

include_directories(src ${INCLIST})

message(STATUS "SRC ${citysim_SRC}")
# Tell CMake to create the helloworld executable
add_executable(citysim "${citysim_SRC}")

add_dependencies(citysim quadtree)

# Use the Widgets module from Qt 5.
target_link_libraries(citysim pthread -lGL Qt5::Core Qt5::Widgets Qt5::3DExtras shp ${GDAL_LIBRARY} ${Boost_LIBRARIES} quadtree)
# Install the executable
install(TARGETS citysim DESTINATION bin)

add_subdirectory(test)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
     --force-new-ctest-process
     --verbose
     --output-on-failure ..
)
