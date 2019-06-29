#ifndef CITYSIM_QUADTREE_TEST_ITEM_H_GUARD
#define CITYSIM_QUADTREE_TEST_ITEM_H_GUARD

#include "quadtree/QuadTreeNodePointItem.h"
#include "util/Point.h"
#include "util/Rect.h"

#include <vector>

class TestItem : public QuadTreeNodePointItem
{
	public:
		TestItem(const Rect &bounds) : QuadTreeNodePointItem("TestItem", bounds) { }
		~TestItem() { }
};


#endif /* CITYSIM_QUADTREE_TEST_ITEM_H_GUARD */
