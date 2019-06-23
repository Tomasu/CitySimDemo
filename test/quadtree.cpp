#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "util/Point.h"
#include "quadtree/QuadTree.h"

#include "quadtree/QuadTreeNodeItem.h"

#include "quadtree/QuadTreeNodeEntityFactory.h"

#include "util/LogUtils.h"
#define TAG "quadtreetest"

class TestItem : public QuadTreeNodeItem
{
	public:
		TestItem(const Rect &bounds) : QuadTreeNodeItem(bounds) { }
		~TestItem() { }

		void buildGeometry(GeometryBuilder * gb) override
		{
			// nada
		}

		std::vector<QuadTreeNodeItem *> split(const Rect & bounds, const Point & crossPoint) override
		{
			return {};
		}

		bool isContainedByRect(const Rect & rect) const override
		{
			for (auto &pt: mPoints)
			{
				if (!rect.contains(pt))
				{
					return false;
				}
			}

			return true;
		}

		bool isPartiallyContainedByRect(const Rect & rect) const override
		{
			bool hasContained = false;
			bool hasNonContained = false;
			for (auto &pt: mPoints)
			{
				if (rect.contains(pt))
				{
					hasContained = true;
				}
				else
				{
					hasNonContained = true;
				}

				if (hasContained && hasNonContained)
				{
					return true;
				}
			}

			return false;
		}

		void addPoint(const Point &pt)
		{
			mPoints.push_back(pt);
		}

		std::string toString() override
		{
			std::stringstream sstr;

			sstr << "TestItem{" << mPoints << "}";

			return sstr.str();
		}

	private:
		std::vector<Point> mPoints;
};

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

	REQUIRE(false);
}
