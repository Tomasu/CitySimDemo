//
// Created by moose on 04/04/19.
//

#include "RoadData.h"

#include "LogUtils.h"

#define TAG "RoadData"

void RoadData::addFwdEdge(TransportGraph::Edge edge, const OGRPoint &begin, const OGRPoint &end)
{
//	log_trace_enter();
	mFwdEdges.push_back(edge);
	mPoints.append(QPointF(begin.getX(), begin.getY()));
	mPoints.append(QPointF(end.getX(), end.getY()));
}

void RoadData::addRevEdge(TransportGraph::Edge edge, const OGRPoint &begin, const OGRPoint &end)
{
//	log_trace_enter();
	mRevEdges.push_back(edge);
	mPoints.append(QPointF(begin.getX(), begin.getY()));
	mPoints.append(QPointF(end.getX(), end.getY()));
}

RoadData::RoadData(const QRectF &bounds, const std::string &name, const std::string &type)
	: QuadTreeNodeItem(bounds), mName(name), mType(type)
{

}

void RoadData::buildGeometry(GeometryBuilder *gb)
{
//	log_trace_enter();
	gb->addLineString(mPoints);
}

