#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "util/Point.h"
#include "quadtree/QuadTree.h"

#include "quadtree/QuadTreeNodeItem.h"

#include "quadtree/QuadTreeNodeEntityFactory.h"

#include "TestItem.h"

#include "util/LogUtils.h"
#define TAG "quadtreetest"

class TestEntityFactory : public QuadTreeNodeEntityFactory
{
	public:
		TestEntityFactory(Qt3DCore::QNode *root)
			: QuadTreeNodeEntityFactory(root)
		{


		}
		QuadTreeNodeEntity * operator()(QuadTreeNode * node) override
		{
			return nullptr;
		}
};

TEST_CASE("test test", "[wat]")
{
	Rect fullRect = Rect(0.0f, 0.0f, 10.0f, 10.0f);
	QuadTree *quadTree = new QuadTree(new TestEntityFactory(nullptr), fullRect, nullptr, 1);

	log_debug("foo");
	quadTree->insertItem(new TestItem(Rect()), true);

	TestItem *testItem1 = new TestItem(Rect());
	testItem1->addPoint({2.0f, 0.0f});
	testItem1->addPoint({2.0f, fullRect.right()});

	quadTree->insertItem(testItem1, true);


	REQUIRE(quadTree->hasSubNodes());
}
