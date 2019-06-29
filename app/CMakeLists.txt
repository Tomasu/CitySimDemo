#cmake_minimum_required(VERSION 3.0)
#project(app VERSION 0.1.0 LANGUAGES CXX)


# Find the QtWidgets library
find_package(Qt53DRender COMPONENTS Private)
find_package(Qt5 CONFIG REQUIRED Core Gui Widgets 3DCore 3DExtras 3DRender 3DRender 3DInput)

find_package(GDAL REQUIRED)

set(SRC
	src/main.cpp
	src/MainWindow.cpp
	src/RoadData.cpp
	src/RootEntity.cpp
	src/ForwardRenderer.cpp
	include/ForwardRenderer.h
	src/aspect/FpsMonitor.cpp
	include/aspect/FpsMonitor.h
	src/aspect/FpsMonitorAspect.cpp
	include/aspect/FpsMonitorAspect.h
	src/aspect/FpsMonitorBackend.cpp
	include/aspect/FpsMonitorComponent.h
	src/aspect/FpsMonitorComponent.cpp
	include/aspect/FpsMonitorMapper.h
	src/aspect/FpsMonitorMapper.cpp
	include/aspect/MovingAverage.h
	src/aspect/MovingAverage.cpp
	include/aspect/UpdateFpsMonitorsJob.h
	src/aspect/UpdateFpsMonitorsJob.cpp
)

add_executable(citysim ${SRC})

target_include_directories(citysim
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${GDAL_INCLUDE_DIRS}
		${Qt53DRender_PRIVATE_INCLUDE_DIRS}
)

message(NOTICE incdir: "${Qt53DRender_PRIVATE_INCLUDE_DIRS}")

add_dependencies(citysim core util quadtree geometryloader graph)
target_compile_options(citysim PRIVATE -fPIC -Wall -Wextra)
target_compile_features(citysim PRIVATE cxx_std_17)
target_link_libraries(citysim
	PRIVATE
		pthread
		GL
		Qt5::Core Qt5::Widgets Qt5::3DExtras
		shp
		${GDAL_LIBRARY}
		Boost::graph
		core util quadtree geometryloader graph
)
