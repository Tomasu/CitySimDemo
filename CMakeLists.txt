cmake_minimum_required(VERSION 3.0)
project(citysim)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to create code from Qt designer ui files
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/home/moose/build/Qt/5.12.2/gcc_64/lib/cmake;${CMAKE_PREFIX_PATH}")
#set(CMAKE_MODULE_PATH "/home/moose/build/Qt/5.12.2/gcc_64/lib/cmake;${CMAKE_MODULE_PATH}")

# Find the QtWidgets library
find_package(Qt5 CONFIG REQUIRED Core Gui Widgets 3DCore 3DExtras 3DRender 3DInput)

set(citysim_SRC
	 src/main.cpp
	 src/MainWindow.cpp src/MainWindow.h src/ShpMeshGeometry.cpp src/ShpMeshGeometry.h src/LocUtil.h)

add_definitions(-fPIC -std=c++11)

list(APPEND INCLIST ${Qt5Core_INCLUDE_DIRS})
list(APPEND INCLIST ${Qt5Widgets_INCLUDE_DIRS})
list(APPEND INCLIST ${Qt5Gui_INCLUDE_DIRS})
list(APPEND INCLIST ${Qt53DCore_INCLUDE_DIRS})
list(APPEND INCLIST ${Qt53DExtras_INCLUDE_DIRS})
list(APPEND INCLIST ${Qt53DRender_INCLUDE_DIRS})
list(APPEND INCLIST ${Qt53DInput_INCLUDE_DIRS})

#list(JOIN INCLIST " " INCLISTVAL)
#message(STATUS INCLIST ${INCLISTVAL})

include_directories(${INCLIST})

# Tell CMake to create the helloworld executable
add_executable(citysim ${citysim_SRC})

# Use the Widgets module from Qt 5.
target_link_libraries(citysim Qt5::Core Qt5::Widgets Qt5::3DExtras shp)

# Install the executable
install(TARGETS citysim DESTINATION bin)
