#include "catch.hpp"

#include "TestItem.h"

#include "util/LogUtils.h"
#define TAG "quadtreepointitemtest"

TEST_CASE("quadtreenodepointitem vertical split", "[quadtree][split]")
{
	Rect fullRect = Rect(0.0f, 0.0f, 10.0f, 10.0f);
	TestItem *testItem1 = new TestItem(Rect());
	testItem1->addSegment({2.0f, fullRect.left()}, {2.0f, fullRect.right()});

	std::vector<QuadTreeNodeItem*> split = testItem1->split(fullRect, fullRect.center());
	QuadTreeNodePointItem *p1 = dynamic_cast<QuadTreeNodePointItem*>(split[0]);
	QuadTreeNodePointItem *p2 = dynamic_cast<QuadTreeNodePointItem*>(split[1]);
	REQUIRE(p1 != nullptr);
	REQUIRE(p2 != nullptr);

	REQUIRE(split.size() == 2);

	std::vector<Point> pts1 = p1->getPoints();
	REQUIRE(pts1.size() == 2);
	REQUIRE(pts1[0] == Point(2, 0, 0));
	REQUIRE(pts1[1] == Point(2, 5, 0));

	std::vector<Point> pts2 = p2->getPoints();
	REQUIRE(pts2.size() == 2);
	REQUIRE(pts2[0] == Point(2, 5, 0));
	REQUIRE(pts2[1] == Point(2, 10, 0));
}

TEST_CASE("quadtreenodepointitem multi-segment split", "[quadtree][split]")
{
	Rect fullRect = Rect(0.0f, 0.0f, 10.0f, 10.0f);
	TestItem *testItem1 = new TestItem(Rect());
	float xpos = fullRect.left() + fullRect.right() - fullRect.width() / 4;
	testItem1->addSegment({fullRect.left(), 2.0f}, {xpos, 2.0f});
	testItem1->addSegment({xpos, 2.0f}, {xpos, fullRect.bottom()});

	std::vector<QuadTreeNodeItem*> split = testItem1->split(fullRect, fullRect.center());
	REQUIRE(split.size() == 3);

	QuadTreeNodePointItem *p1 = dynamic_cast<QuadTreeNodePointItem*>(split[0]);
	QuadTreeNodePointItem *p2 = dynamic_cast<QuadTreeNodePointItem*>(split[1]);
	QuadTreeNodePointItem *p3 = dynamic_cast<QuadTreeNodePointItem*>(split[2]);
	REQUIRE(p1 != nullptr);
	REQUIRE(p2 != nullptr);
	REQUIRE(p3 != nullptr);

	REQUIRE(p1->getNumPoints() == 2);
	REQUIRE(p1->pointAt(0) == Point(0, 2, 0));
	REQUIRE(p1->pointAt(1) == Point(5, 2, 0));

	REQUIRE(p2->getNumPoints() == 4);
	REQUIRE(p2->pointAt(0) == Point(5, 2, 0));
	REQUIRE(p2->pointAt(1) == Point(xpos, 2, 0));
	REQUIRE(p2->pointAt(2) == Point(xpos, 2, 0));
	REQUIRE(p2->pointAt(3) == Point(xpos, 5, 0));

	REQUIRE(p3->getNumPoints() == 2);
	REQUIRE(p3->pointAt(0) == Point(xpos, 5, 0));
	REQUIRE(p3->pointAt(1) == Point(xpos, 10, 0));
}

#include "core/Line.h"

TEST_CASE("line length spliut test", "[split]")
{
	Point p1 = {0, 0}, p2 = {5.000001,5.000001}, p3 = {10, 10};
	Line origLine = {p1, p3};
	float origLen = origLine.length();


	Line line1 = {p1, p2}, line2 = {p2, p3};
	float line1Len = line1.length(), line2Len = line2.length();
	float totalLen = line1Len + line2Len;

	log_debug("origLen: %s, line1: %s, line2: %s, total: %s", origLen, line1Len, line2Len, totalLen);
	REQUIRE(origLen == totalLen);
}
