set(CTEST_PROJECT_NAME "CitySim")

set(SRC
	quadtree.cpp
	quadtreenodepointitem.cpp
	${TEST_BASE_SRC}
)

add_executable(quadtreetest ${SRC})
target_include_directories(quadtreetest
	PRIVATE
		${TEST_BASE_DIR}
)


target_link_libraries(quadtreetest quadtree core util Qt5::Core Qt5::3DExtras Qt5::3DRender ${GDAL_LIBRARY})
add_dependencies(quadtreetest quadtree core util)
add_test(quadtree quadtreetest)
