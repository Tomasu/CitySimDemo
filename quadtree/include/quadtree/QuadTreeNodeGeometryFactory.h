//
// Created by moose on 05/04/19.
//

#ifndef CITYSIM_QUADTREENODEGEOMETRYFACTORY_H
#define CITYSIM_QUADTREENODEGEOMETRYFACTORY_H

#include "util/Rect.h"

#include <Qt3DRender/QGeometryFactory>

class QuadTreeNode;

class QuadTreeNodeGeometryFactory : public Qt3DRender::QGeometryFactory
{
	public:
    struct NodeInfo
    {
        explicit NodeInfo(const QuadTreeNode *node);
        Rect mNodeBounds;
        bool mNodeHasSubNodes;
        uint32_t mNodeItemCount;

        bool operator==(const NodeInfo &other) const;

        void update(const QuadTreeNode *node);
    };

		explicit QuadTreeNodeGeometryFactory(QuadTreeNode *node);

		Qt3DRender::QGeometry *operator()() override;

		virtual bool operator==(const Qt3DRender::QGeometryFactory &other) const;

		QT3D_FUNCTOR(QuadTreeNodeGeometryFactory)

	private:


		NodeInfo mNodeInfo;

		QuadTreeNode *mNode;


};



#endif //CITYSIM_QUADTREENODEGEOMETRYFACTORY_H
