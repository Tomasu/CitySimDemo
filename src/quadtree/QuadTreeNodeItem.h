//
// Created by moose on 04/04/19.
//

#ifndef CITYSIM_QUADTREENODEITEM_H
#define CITYSIM_QUADTREENODEITEM_H

#include <memory>
#include <cstdint>
#include <vector>
#include <QtCore/QRectF>
#include <src/BufferBuilder.h>
#include <src/Constants.h>
#include <src/GeometryBuilder.h>


class QuadTreeNodeItem
{
	public:
		QuadTreeNodeItem() = default;

		explicit QuadTreeNodeItem(const QRectF &bounds)
			: mBounds{bounds}
		{

		}

		virtual ~QuadTreeNodeItem() = default;

		virtual void buildGeometry(GeometryBuilder *gb) = 0;

		const QRectF &bounds() { return mBounds; }

	private:
		QRectF mBounds;
};


#endif //CITYSIM_QUADTREENODEITEM_H
