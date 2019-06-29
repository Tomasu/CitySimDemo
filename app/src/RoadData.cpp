//
// Created by moose on 04/04/19.
//

#include "RoadData.h"


#include "quadtree/QuadTreeNode.h"
#include "core/LineString.h"
#include "core/GeometryBuilder.h"
#include "util/Plane3D.h"
#include "util/Rect.h"
#include "core/Line.h"
#include <QLineF>

#include "util/LogUtils.h"
#define TAG "RoadData"

RoadData::RoadData(const Rect &bounds, const std::string &name, const std::string &type)
: QuadTreeNodePointItem(name, bounds), mType(type)
{

}

RoadData::RoadData(const Rect& bounds, const std::string& name, const std::string& type, const std::vector<Point>& points)
	: QuadTreeNodePointItem(name, bounds, points), mType(type)
{
}


void RoadData::addFwdEdge(const TransportGraph::Edge &edge, const Point &begin, const Point &end)
{
//	log_trace_enter();
	mFwdEdges.push_back(edge);
// 	mPoints.append(begin);
// 	mPoints.append(end);
}

void RoadData::addRevEdge(const TransportGraph::Edge &edge, const Point &begin, const Point &end)
{
//	log_trace_enter();
	mRevEdges.push_back(edge);
// 	mPoints.append(begin);
// 	mPoints.append(end);
}

template<>
std::string AnyToString<RoadData*>(RoadData * const &d)
{
	std::stringstream sstr;

	sstr << "RoadData{name=" << d->getName() << ", pts=" << std::fixed << d->getPoints() << "}";

	return sstr.str();
}


std::string RoadData::toString()
{
	return AnyToString(this);
}


#include <string>
#include <sstream>



std::ostream & operator << (std::ostream &out, RoadData * const &d)
{
	out << AnyToString(d);

	return out;
}
