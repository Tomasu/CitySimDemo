//
// Created by moose on 05/04/19.
//

#ifndef CITYSIM_QUADTREENODEGEOMETRY_H
#define CITYSIM_QUADTREENODEGEOMETRY_H


#include <Qt3DRender/QGeometry>
#include <QtCore/QRectF>

#include "quadtree/QuadTreeNode.h"

namespace Qt3DRender {
	class QAttribute;
	class QBuffer;
}

class QuadTreeNodeGeometry : public Qt3DRender::QGeometry
{
	public:
		explicit QuadTreeNodeGeometry(QuadTreeNode *node, Qt3DCore::QNode *parent = nullptr);
		~QuadTreeNodeGeometry();

		int getNumVerticies() const { return mNumVerticies; }

		QuadTreeNode *node() const { return mNode; }

		const QRectF &bounds() const { return mNode->bounds(); }
		double originX() const { return bounds().left(); }
		double originY() const { return bounds().top(); }

		QByteArray generateVertexData();
		QByteArray generateIndexData();

		virtual bool operator==(const QuadTreeNodeGeometry &other) const;

	private:
		QuadTreeNode *mNode;
		int mNumVerticies;

		Qt3DRender::QAttribute *mPosAttribute;
		Qt3DRender::QAttribute *mColorAttribute;
		//Qt3DRender::QAttribute *mIndexAttribute;
		Qt3DRender::QBuffer *mVertexBuffer;
		Qt3DRender::QBuffer *mIndexBuffer;

};


#endif //CITYSIM_QUADTREENODEGEOMETRY_H
