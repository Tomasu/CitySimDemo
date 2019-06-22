//
// Created by moose on 04/04/19.
//

#ifndef CITYSIM_QUADTREENODEITEM_H
#define CITYSIM_QUADTREENODEITEM_H

#include "util/Rect.h"
#include "util/Point.h"
#include <QVector>

#include <string>

class GeometryBuilder;
class QuadTreeNode;

class QuadTreeNodeItem
{
	public:
		QuadTreeNodeItem() = default;

		explicit QuadTreeNodeItem(const Rect &bounds)
			: mBounds{bounds}
		{

		}

		virtual ~QuadTreeNodeItem() = default;

		virtual void buildGeometry(GeometryBuilder *gb) = 0;

		const Rect &bounds() { return mBounds; }

		const Point &offset() { return mOffset; }
		void setOffset(const Point &offset) { mOffset = offset; }

		virtual std::vector<QuadTreeNodeItem *> split(const Rect &bounds, const Point &crossPoint) = 0;

		virtual bool isPartiallyContainedByRect(const Rect &rect) const = 0;

		virtual bool isContainedByRect(const Rect &rect) const = 0;

		virtual std::string toString() = 0;

	protected:
		Point mOffset;
		Rect mBounds;
};


#endif //CITYSIM_QUADTREENODEITEM_H
