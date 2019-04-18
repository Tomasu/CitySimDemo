//
// Created by moose on 05/04/19.
//

#ifndef CITYSIM_QUADTREENODEGEOMETRYFACTORY_H
#define CITYSIM_QUADTREENODEGEOMETRYFACTORY_H


#include <Qt3DRender/QGeometryFactory>
#include "QuadTreeNodeGeometry.h"

#include "QuadTreeNode.h"

class QuadTreeNodeGeometryFactory : public Qt3DRender::QGeometryFactory
{
	public:
		explicit QuadTreeNodeGeometryFactory(QuadTreeNode *node);

		Qt3DRender::QGeometry *operator()() override;

		virtual bool operator==(const Qt3DRender::QGeometryFactory &other) const;

		QT3D_FUNCTOR(QuadTreeNodeGeometryFactory)

	private:
		struct NodeInfo
		{
			explicit NodeInfo(const QuadTreeNode *node);
			const QRectF &mNodeBounds;
			bool mNodeHasSubNodes;
			uint32_t mNodeItemCount;

			bool operator==(const NodeInfo &other) const;
		};

		NodeInfo mNodeInfo;

		QuadTreeNode *mNode;


};



#endif //CITYSIM_QUADTREENODEGEOMETRYFACTORY_H
