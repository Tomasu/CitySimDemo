//
// Created by moose on 04/04/19.
//

#ifndef CITYSIM_QUADTREE_H
#define CITYSIM_QUADTREE_H

#include "quadtree/QuadTreeNode.h"

#include "util/Rect.h"

class QuadTree : public QuadTreeNode
{
	public:
		QuadTree(const QuadTreeNode&) = delete;

		explicit QuadTree(QuadTreeNodeEntityFactory *entityFactory, const Rect &bounds, QuadTreeNodeMapper *mapper, size_t nodeSplitCount = QuadTreeNode::NODE_SPLIT_COUNT);
		virtual ~QuadTree();

		QuadTreeNode *findNode(const Point &vec);

		void dumpTree();

	private:

};

#endif //CITYSIM_QUADTREE_H
