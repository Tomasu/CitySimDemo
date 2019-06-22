//
// Created by moose on 05/04/19.
//

#ifndef CITYSIM_QUADTREENODEGEOMETRY_H
#define CITYSIM_QUADTREENODEGEOMETRY_H

#include "util/Rect.h"

#include <Qt3DRender/QGeometry>

#include <QMutex>

class QuadTreeNode;

namespace Qt3DRender {
	class QAttribute;
	class QBuffer;
}

class QuadTreeNodeGeometry : public Qt3DRender::QGeometry
{
	Q_OBJECT

	public:

		explicit QuadTreeNodeGeometry(QuadTreeNode *node, Qt3DCore::QNode *parent = nullptr);
		~QuadTreeNodeGeometry();

		int getNumVerticies() const { return mNumVerticies; }

		QuadTreeNode *node() const { return mNode; }

		const Rect &bounds() const;
		double originX() const { return bounds().left(); }
		double originY() const { return bounds().top(); }

		QByteArray generateVertexData();
		QByteArray generateIndexData();

		virtual bool operator==(const QuadTreeNodeGeometry &other) const;

    protected:
      void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change);

	private:
		static int sId;
		int mId;
		QuadTreeNode *mNode;
		int mNumVerticies;

		Qt3DRender::QAttribute *mPosAttribute;
		Qt3DRender::QAttribute *mNormalAttribute;
		Qt3DRender::QAttribute *mColorAttribute;
		//Qt3DRender::QAttribute *mIndexAttribute;
		Qt3DRender::QBuffer *mVertexBuffer;
		Qt3DRender::QBuffer *mIndexBuffer;

      void initialize();

		static QMutex sMutex;

};


#endif //CITYSIM_QUADTREENODEGEOMETRY_H
