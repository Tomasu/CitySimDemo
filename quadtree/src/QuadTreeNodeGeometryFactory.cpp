//
// Created by moose on 05/04/19.
//

#include "QuadTreeNode.h"
#include "QuadTreeNodeGeometry.h"
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
	QuadTreeNodeGeometry *geometry = mNode->createGeometry();

	mNodeInfo.update(mNode);

	log_debug("num verts: %s", geometry->getNumVerticies());

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
		log_debug("match=%s  %s  vs  %s", match, mNodeInfo, otherFunctor->mNodeInfo);
		return match;
	}
	else
	{
		log_debug("no other functor??");
	}

	return false;
}

QuadTreeNodeGeometryFactory::NodeInfo::NodeInfo(const QuadTreeNode *node)
		: mNodeBounds{node->bounds()},
		  mNodeHasSubNodes{node->hasSubNodes()},
		  mNodeItemCount{node->getItemCount()}
{

}

void QuadTreeNodeGeometryFactory::NodeInfo::update(const QuadTreeNode *node)
{
	mNodeBounds = node->bounds();
	mNodeHasSubNodes = node->hasSubNodes();
	mNodeItemCount = node->getItemCount();
}


bool QuadTreeNodeGeometryFactory::NodeInfo::operator==(const QuadTreeNodeGeometryFactory::NodeInfo &other) const
{
	return other.mNodeBounds == mNodeBounds
			 && other.mNodeHasSubNodes == mNodeHasSubNodes
			 && other.mNodeItemCount == mNodeItemCount
			;
}

#include <sstream>

template <>
std::string AnyToString<QuadTreeNodeGeometryFactory::NodeInfo>(const QuadTreeNodeGeometryFactory::NodeInfo &info)
{
	std::stringstream sstr;

	sstr << "NodeInfo{bounds=" << info.mNodeBounds
		<< ", hasSubNodes=" << info.mNodeHasSubNodes
		<< ", itemCount=" << info.mNodeItemCount
		<< "}";

	return sstr.str();
}

