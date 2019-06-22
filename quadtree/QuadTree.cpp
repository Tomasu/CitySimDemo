//
// Created by moose on 04/04/19.
//

#include "QuadTree.h"
#include "LogUtils.h"

#include <cstdint>
#include "util/Rect.h"

#define TAG "QuadTree"

QuadTree::QuadTree(QuadTreeNodeEntityFactory* entityFactory, const Rect &bounds, QuadTreeNodeMapper* mapper, size_t nodeSplitCount)
	: QuadTreeNode(nullptr, entityFactory, bounds, mapper, nodeSplitCount)
{
}

QuadTree::~QuadTree()
{
}

QuadTreeNode *QuadTree::findNode(const QVector3D& vec)
{
	std::vector<QuadTreeNode*> queue(subNodes());

	while(!queue.empty())
	{
		QuadTreeNode *node = queue.back();
		queue.pop_back();

		log_debug("node: @ %s", node->bounds());

		if (node->bounds().contains(vec.x(), vec.y()))
		{
			log_debug("contains vec!");
			// if we have sub nodes, we don't have items.
			//  so just push sub nodes onto queue and continue.
			if (node->hasSubNodes())
			{
				log_debug("has subnodes, check children");
				auto subNodes = node->subNodes();
				queue.insert(queue.end(), subNodes.begin(), subNodes.end());
			}
			else
			{
				log_debug("return node! ch=%s", node->getItemCount());
				// no subnodes means we found our node
				return node;
			}
		}
	}

	return nullptr;
}

QuadTree::QuadTreeNode * QuadTree::findNode(const QPointF& pt)
{
	return findNode(QVector3D{(float)pt.x(), (float)pt.y(), 0.0f});
}


void QuadTree::dumpTree()
{
	dumpNode("");
}

