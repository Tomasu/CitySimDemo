#ifndef CITYSIM_QUADTREE_TEST_ITEM_H_GUARD
#define CITYSIM_QUADTREE_TEST_ITEM_H_GUARD

#include "quadtree/QuadTreeNodeItem.h"
#include "util/Point.h"
#include "util/Rect.h"

#include <vector>

#define TAG "TestItem"
#include "util/LogUtils.h"

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


#endif /* CITYSIM_QUADTREE_TEST_ITEM_H_GUARD */
