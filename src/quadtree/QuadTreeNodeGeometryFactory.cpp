//
// Created by moose on 05/04/19.
//

#include "QuadTreeNode.h"
#include "QuadTreeNodeGeometryFactory.h"
#include "LogUtils.h"

#define TAG "QuadTreeNodeGeometryFactory"

QuadTreeNodeGeometryFactory::QuadTreeNodeGeometryFactory(QuadTreeNode *node)
		: mNodeInfo{node}, mNode{node}
{
	log_trace_enter();
}

Qt3DRender::QGeometry *QuadTreeNodeGeometryFactory::operator()()
{
	log_trace_enter();
	Qt3DRender::QGeometry *geometry = mNode->createGeometry();
	log_trace_exit();
	return geometry;
}

bool QuadTreeNodeGeometryFactory::operator==(const Qt3DRender::QGeometryFactory &other) const
{
	log_trace_enter();

	const auto *otherFunctor = functor_cast<QuadTreeNodeGeometryFactory>(&other);
	if (otherFunctor != nullptr)
	{
		bool match = (otherFunctor->mNode == mNode && otherFunctor->mNodeInfo == mNodeInfo);
		log_debug("match=%s", match);
		return match;
	}

	return false;
}

QuadTreeNodeGeometryFactory::NodeInfo::NodeInfo(const QuadTreeNode *node)
		: mNodeBounds{node->bounds()},
		  mNodeHasSubNodes{node->hasSubNodes()},
		  mNodeItemCount{node->getItemCount()}
{

}

bool QuadTreeNodeGeometryFactory::NodeInfo::operator==(const QuadTreeNodeGeometryFactory::NodeInfo &other) const
{
	return other.mNodeBounds == mNodeBounds
			 && other.mNodeHasSubNodes == mNodeHasSubNodes
			 && other.mNodeItemCount == mNodeItemCount
			;
}
