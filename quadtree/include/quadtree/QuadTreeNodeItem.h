#ifndef CITYSIM_QUADTREENODEITEM_H
#define CITYSIM_QUADTREENODEITEM_H

//
// Created by moose on 04/04/19.
//

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

		explicit QuadTreeNodeItem(const std::string &name, const Rect &bounds)
			: mName{name}, mBounds{bounds}
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

		virtual float length() const = 0;

		virtual std::string toString() = 0;

		virtual std::string getName() const { return mName; };

	protected:
		std::string mName;
		Point mOffset;
		Rect mBounds;
};


#endif //CITYSIM_QUADTREENODEITEM_H
