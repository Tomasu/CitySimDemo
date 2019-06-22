//
// Created by moose on 06/04/19.
//

#ifndef CITYSIM_QUADTREENODEENTITYFACTORY_H
#define CITYSIM_QUADTREENODEENTITYFACTORY_H

namespace Qt3DCore {
	class QNode;
}

class QuadTreeNode;
class QuadTreeNodeEntity;
class QuadTreeNodeEntityFactory
{
	public:
		explicit QuadTreeNodeEntityFactory(Qt3DCore::QNode *root)
			: mRootNode(root) { }

		virtual ~QuadTreeNodeEntityFactory() {};

		virtual QuadTreeNodeEntity *operator()(QuadTreeNode *node) = 0;

		Qt3DCore::QNode *rootNode() { return mRootNode; }

	private:
		Qt3DCore::QNode *mRootNode;
};

#include "QuadTreeNodeEntity.h"

#endif //CITYSIM_QUADTREENODEENTITYFACTORY_H
