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

TEST_CASE("quadtree test", "[quadtree]")
{
	Rect fullRect = Rect(0.0f, 0.0f, 10.0f, 10.0f);
	QuadTree *quadTree = new QuadTree(new TestEntityFactory(nullptr), fullRect, nullptr, 0);

	//log_debug("foo");
	//quadTree->insertItem(new TestItem(Rect()), true);

	TestItem *testItem1 = new TestItem(Rect());
	testItem1->addSegment({2.0f, fullRect.left()}, {2.0f, fullRect.right()});

	quadTree->insertItem(testItem1, true);

	REQUIRE(quadTree->hasSubNodes());

	std::vector<QuadTreeNode*> subNodes = quadTree->subNodes();
	REQUIRE(subNodes.size() == 4);

	QuadTreeNode *topLeft = quadTree->topLeft();
	REQUIRE(topLeft->getItemCount() == 1);

	QuadTreeNode *bottomLeft = quadTree->bottomLeft();
	REQUIRE(bottomLeft->getItemCount() == 1);

	QuadTreeNodePointItem *item1 = topLeft->itemAs<QuadTreeNodePointItem>(0);
	REQUIRE(item1->pointAt(0) == Point{2, 0, 0});
	REQUIRE(item1->pointAt(1) == Point{2, 5, 0});

	QuadTreeNodePointItem *item2 = bottomLeft->itemAs<QuadTreeNodePointItem>(0);
	REQUIRE(item2->pointAt(0) == Point{2, 5, 0});
	REQUIRE(item2->pointAt(1) == Point{2, 10, 0});
}


TEST_CASE("quadtree multi-segment split", "[quadtree][split]")
{
	Rect fullRect = Rect(0.0f, 0.0f, 10.0f, 10.0f);
	QuadTree *quadTree = new QuadTree(new TestEntityFactory(nullptr), fullRect, nullptr, 0);

	TestItem *testItem1 = new TestItem(Rect());
	float xpos = fullRect.left() + fullRect.right() - fullRect.width() / 4;
	testItem1->addSegment({fullRect.left(), 2.0f}, {xpos, 2.0f});
	testItem1->addSegment({xpos, 2.0f}, {xpos, fullRect.bottom()});

	quadTree->insertItem(testItem1);

	QuadTreeNode *topLeft = quadTree->topLeft();
	QuadTreeNode *topRight = quadTree->topRight();
	QuadTreeNode *bottomRight = quadTree->bottomRight();

	REQUIRE(topLeft->getItemCount() == 1);
	QuadTreeNodePointItem *topLeftItem = topLeft->itemAs<QuadTreeNodePointItem>(0);

	REQUIRE(topLeftItem->pointAt(0) == Point(0, 2, 0));
	REQUIRE(topLeftItem->pointAt(1) == Point(5, 2, 0));

	REQUIRE(topRight->getItemCount() == 1);
	QuadTreeNodePointItem *topRightItem = topRight->itemAs<QuadTreeNodePointItem>(0);

	REQUIRE(topRightItem->pointAt(0) == Point(5, 2, 0));
	REQUIRE(topRightItem->pointAt(1) == Point(xpos, 2, 0));
	REQUIRE(topRightItem->pointAt(2) == Point(xpos, 2, 0));
	REQUIRE(topRightItem->pointAt(3) == Point(xpos, 5, 0));

	REQUIRE(bottomRight->getItemCount() == 1);
	QuadTreeNodePointItem *bottomRightItem = bottomRight->itemAs<QuadTreeNodePointItem>(0);

	REQUIRE(bottomRightItem->pointAt(0) == Point(xpos, 5, 0));
	REQUIRE(bottomRightItem->pointAt(1) == Point(xpos, 10, 0));
}

TEST_CASE("quadtree multi-segment2 split", "[quadtree][split]")
{
	Rect fullRect = Rect(0.0f, 0.0f, 10.0f, 10.0f);
	QuadTree *quadTree = new QuadTree(new TestEntityFactory(nullptr), fullRect, nullptr, 0);

	TestItem *testItem1 = new TestItem(Rect());
	float xpos = fullRect.left() + fullRect.right() - fullRect.width() / 4;
	testItem1->addSegment({fullRect.left(), 2.0f}, {xpos/2, 2.0f});
	testItem1->addSegment({xpos/2, 2.0f}, {xpos, 2.0f});
	float ypos2 = fullRect.top() + fullRect.width() / 4 * 3;
	testItem1->addSegment({xpos, 2.0f}, {xpos, ypos2});
	testItem1->addSegment({xpos, ypos2}, {xpos, fullRect.bottom()});

	quadTree->insertItem(testItem1);

	QuadTreeNode *topLeft = quadTree->topLeft();
	QuadTreeNode *topRight = quadTree->topRight();
	QuadTreeNode *bottomRight = quadTree->bottomRight();

	REQUIRE(topLeft->getItemCount() == 1);
	QuadTreeNodePointItem *topLeftItem = topLeft->itemAs<QuadTreeNodePointItem>(0);

	REQUIRE(topLeftItem->pointAt(0) == Point(0, 2, 0));
	REQUIRE(topLeftItem->pointAt(1) == Point(3.75, 2, 0));
	REQUIRE(topLeftItem->pointAt(2) == Point(3.75, 2, 0));
	REQUIRE(topLeftItem->pointAt(3) == Point(5, 2, 0));

	REQUIRE(topRight->getItemCount() == 1);
	QuadTreeNodePointItem *topRightItem = topRight->itemAs<QuadTreeNodePointItem>(0);

	REQUIRE(topRightItem->pointAt(0) == Point(5, 2, 0));
	REQUIRE(topRightItem->pointAt(1) == Point(xpos, 2, 0));
	REQUIRE(topRightItem->pointAt(2) == Point(xpos, 2, 0));
	REQUIRE(topRightItem->pointAt(3) == Point(xpos, 5, 0));

	REQUIRE(bottomRight->getItemCount() == 1);
	QuadTreeNodePointItem *bottomRightItem = bottomRight->itemAs<QuadTreeNodePointItem>(0);

	REQUIRE(bottomRightItem->pointAt(0) == Point(xpos, 5, 0));
	REQUIRE(bottomRightItem->pointAt(1) == Point(xpos, ypos2, 0));

	REQUIRE(bottomRightItem->pointAt(2) == Point(xpos, ypos2, 0));
	REQUIRE(bottomRightItem->pointAt(3) == Point(xpos, 10, 0));
}
