
set(CMAKE_PREFIX_PATH "/home/moose/build/Qt/5.12.3/gcc_64/lib/cmake;${CMAKE_PREFIX_PATH}")

# Find the QtWidgets library
find_package(Qt5 CONFIG REQUIRED Core Gui Widgets 3DCore 3DExtras 3DRender 3DInput)

find_package(GDAL REQUIRED)

