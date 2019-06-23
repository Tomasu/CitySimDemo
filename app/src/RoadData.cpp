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
: QuadTreeNodeItem(bounds), mName(name), mType(type)
{

}

RoadData::RoadData(const Rect& bounds, const std::string& name, const std::string& type, const std::vector<Point>& points)
	: QuadTreeNodeItem(bounds), mName(name), mType(type), mPoints(points)
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

void RoadData::addSegment(const Point& start, const Point& end)
{
	mPoints.push_back(start);
	mPoints.push_back(end);
}


void RoadData::buildGeometry(GeometryBuilder *gb)
{
	//log_trace_enter();

// 	QVector<Point> adjPoints = mPoints;
// 	for (Point &pt: adjPoints)
// 	{
// 		pt += mOffset;
// 	}

//	gb->addLineString(adjPoints);

	gb->addLineString(LineString(mPoints));
}

#include <limits>
static Rect calcBounds(const std::vector<Point> &items)
{
	Rect bounds;

	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::min();
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::min();

	for (Point vec: items)
	{
		if (vec.x() < minX)
		{
			minX = vec.x();
		}

		if (vec.x() > maxX)
		{
			maxX = vec.x();
		}

		if (vec.y() < minY)
		{
			minY = vec.y();
		}

		if (vec.y() > maxY)
		{
			maxY = vec.y();
		}
	}

	return Rect(minX, minY, maxX-minX, maxY-minY);
}

bool RoadData::isPartiallyContainedByRect(const Rect& rect) const
{
// 	if (rect.contains(mBounds))
// 	{
// 		return false;
// 	}

	bool atLeastOneContained = false;
	bool atLeastOneNonContained = false;
	Point lastNonContained;
	Point lastContained;

	Rect adjRect = rect;
	adjRect.setWidth(rect.size().width()-0.0000001);
	adjRect.setHeight(rect.size().height()-0.000001);

	for(const Point &vec: mPoints)
	{
		// rect.contains(vec.x(), vec.y());
		bool contained = adjRect.contains(vec);
		if (!contained)
		{
			lastNonContained = vec;
			atLeastOneNonContained = true;

			log_debug("got non contained point. %s -> %s", vec, adjRect);
			if(atLeastOneContained)
			{
// 				log_debug("had previously contained point cur=%s cont=%s rect=%s", vec, lastContained,  adjRect);
				return true;
			}
		}
		else
		{
			log_debug("got contained point: %s -> %s", vec, adjRect);

			lastContained = vec;
			if (atLeastOneNonContained)
			{
// 				log_debug("got at least one contained and non contained. cur=%s non=%s rect=%s", vec, lastNonContained, adjRect);
				return true;
			}

			atLeastOneContained = true;
		}
	}

	return false;
}

bool RoadData::isContainedByRect(const Rect& rect) const
{
// 	Rect adjRect = rect;
// 	adjRect.setWidth(rect.size().width()-0.01);
// 	adjRect.setHeight(rect.size().height()-0.01);

	for (const Point &point: mPoints)
	{
		if (!rect.contains(point))
		{
			return false;
		}
	}

	return true;
}

// TODO: this needs to take into account the transport graph nodes as well.. for now it doesnt...
//  probably the graph will be tied into the quadtree closer...
// user should ideally not call this if the bounds of this item is entirely in bounds to start with...
/*
 * bounds is the parent quadtreenode's bounds we want to split into subnodes..
 * crossPoint is the center point of the parent quadtreenode
 */
std::vector<QuadTreeNodeItem*> RoadData::split(const Rect& bounds, const Point& crossPoint)
{
	//log_debug("split %s in %s", mBounds, bounds);

	std::vector<QuadTreeNodeItem*> newItems;

	RoadData *newItem = nullptr;

	std::vector<Point> curPoints;

	Size subRectSize = bounds.size()/2;

	Rect topLeft = Rect(bounds.topLeft(), subRectSize);
	Rect topRight = Rect(Point(bounds.center().x(), bounds.top()), subRectSize);
	Rect bottomLeft = Rect(Point(bounds.left(), bounds.center().y()), subRectSize);
	Rect bottomRight = Rect(bounds.center(), subRectSize);

	Point vertV0 = Point(bounds.center().x(), bounds.top(), 0.0f);
	Point vertV1 = Point(bounds.center().x(), bounds.bottom(), 0.0f);
	Point vertV2 = Point(bounds.center().x(), bounds.center().y(), 1000.0f);
	//Plane3D vertPlane = {vertV0, vertV1, vertV2};

	Point horizV0 = Point(bounds.left(), bounds.center().y(), 0.0f);
	Point horizV1 = Point(bounds.right(), bounds.center().y(), 0.0f);
	Point horizV2 = Point(bounds.center().x(), bounds.center().y(), 1000.0f);
	//Plane3D horizPlane = {horizV0, horizV1, horizV2};

	Line vertLine = Line(Point(bounds.center().x(), bounds.top()), Point(bounds.center().x(), bounds.bottom()));
	Line horizLine = Line(Point(bounds.left(), bounds.center().y()), Point(bounds.right(), bounds.center().y()));

	int numPoints = mPoints.size();

	std::vector<Point> points = mPoints;

// 	for (auto &pt: mPoints)
// 	{
// 		log_debug("pt: %s", pt);
// 	}

	for(size_t i = 0; i < points.size()-1; i+=2)
	{
		Point sVec = points[i];
		Point eVec = points[i+1];

		log_debug("segment: %s", Line{sVec, eVec});

		// add current point
// 		log_debug("add svec: %s", sVec);

		curPoints.push_back(sVec);

		Point vertIntersectPt;
		Point horizIntersectPt;
// 		bool vertIntersection = vertPlane.intersects(sVec, eVec, &vertIntersectPt) == Plane3D::IntersectUnique;
// 		bool horizIntersection = horizPlane.intersects(sVec, eVec, &horizIntersectPt) == Plane3D::IntersectUnique;

		Line curLine = Line(sVec, eVec);

		int vertIntersectCount = 0;

		Point vertIntersectEndPt;
		Line::IntersectType vertIntersection = curLine.intersect(vertLine, &vertIntersectPt, &vertIntersectEndPt);
		if (vertIntersection == Line::Intersect || vertIntersection == Line::Overlap)
		{
			if (vertIntersection == Line::Intersect)
			{
				log_debug("vert %s %s pt1=%s vert-line=%s", vertIntersection, curLine, vertIntersectPt, vertLine);
			}
			else
			{
				log_debug("vert %s %s pt1=%s pt2=%s vert-line=%s", vertIntersection, curLine, vertIntersectPt, vertIntersectEndPt, vertLine);
			}

			bool topLeftContains = topLeft.contains(vertIntersectPt);
			bool topRightContains = topRight.contains(vertIntersectPt);
			bool bottomLeftContains = bottomLeft.contains(vertIntersectPt);
			bool bottomRightContains = bottomRight.contains(vertIntersectPt);
			vertIntersectCount = topLeftContains + topRightContains + bottomLeftContains + bottomRightContains;

// 			if (topLeftContains)
// 			{
// 				log_debug("vert intersect in topLeft %s", topLeft);
// 			}
// 			if (topRightContains)
// 			{
// 				log_debug("vert intersect in topRight %s", topRight);
// 			}
// 			if (bottomLeftContains)
// 			{
// 				log_debug("vert intersect in bottomLeft %s", bottomLeft);
// 			}
// 			if (bottomRightContains)
// 			{
// 				log_debug("vert intersect in bottomRight %s", bottomRight);
// 			}
		}

		int horizIntersectCount = 0;

		Point horizIntersectEndPt;
		Line::IntersectType horizIntersection = curLine.intersect(horizLine, &horizIntersectPt, &horizIntersectEndPt);
		if (horizIntersection == Line::Intersect || horizIntersection == Line::Overlap)
		{
			if (horizIntersection == Line::Intersect)
			{
				log_debug("horiz %s %s pt1=%s horiz-line=%s",
							 horizIntersection, curLine,
				  horizIntersectPt, horizLine);
			}
			else
			{
				log_debug("horiz %s %s pt1=%s pt2=%s horiz-line=%s",
							 horizIntersection, curLine,
				  horizIntersectPt, horizIntersectEndPt, horizLine);
			}

			bool topLeftContains = topLeft.contains(horizIntersectPt);
			bool topRightContains = topRight.contains(horizIntersectPt);
			bool bottomLeftContains = bottomLeft.contains(horizIntersectPt);
			bool bottomRightContains = bottomRight.contains(horizIntersectPt);
			horizIntersectCount = topLeftContains + topRightContains + bottomLeftContains + bottomRightContains;

// 			if (topLeftContains)
// 			{
// 				log_debug("horiz intersect in topLeft %s", topLeft);
// 			}
// 			if (topRightContains)
// 			{
// 				log_debug("horiz intersect in topRight %s", topRight);
// 			}
// 			if (bottomLeftContains)
// 			{
// 				log_debug("horiz intersect in bottomLeft %s", bottomLeft);
// 			}
// 			if (bottomRightContains)
// 			{
// 				log_debug("horiz intersect in bottomRight %s", bottomRight);
// 			}
		}

		if (vertIntersectPt == sVec)
		{
			log_debug("vert intersect point matches sVec?");
		}
		 if (vertIntersectPt == eVec)
		{
			log_debug("vert intersect point matches eVec?");
		}

		if (horizIntersectPt == sVec)
		{
			log_debug("horiz intersect point matches sVec?");
		}
		if (horizIntersectPt == eVec)
		{
			log_debug("horiz intersect point matches eVec?");
		}

// 		log_debug("intersect count: %s", vertIntersectCount + horizIntersectCount);

		if ((vertIntersection && vertIntersectPt != sVec && vertIntersectPt != eVec)
			|| (horizIntersection && horizIntersectPt != sVec && horizIntersectPt != eVec))
		{
			Point intersectPt = vertIntersection ? vertIntersectPt : horizIntersectPt;

			//log_debug(" intersection at %s (between %s and %s with %s plane)", intersectPt, sVec, eVec, vertIntersection ? "vert" : "horiz");

			// woo, we have an intersection :D
			// add new QuadTreeNodeItem to list!
// 			log_debug("add intersectPt: %s", intersectPt);

			curPoints.push_back(intersectPt);
			RoadData *newItem = new RoadData(calcBounds(curPoints), getName(), getType(), curPoints);
			//log_debug("add new Item: %s", newItem);
			newItems.push_back(newItem);

			// reset points, and add this point for the next run
			curPoints.clear();
// 			log_debug("add intersectPt to new list: %s", intersectPt);
			curPoints.push_back(intersectPt);
		}

// 		log_debug("add eVec: %s", eVec);

		curPoints.push_back(eVec);
	}

	// if we have some points left over, create new item for them
	if (!curPoints.empty())
	{
		RoadData *newItem = new RoadData(calcBounds(curPoints), getName(), getType(), curPoints);
		//log_debug("last add newItem: %s", newItem);
		newItems.push_back(newItem);
	}

	//log_trace_exit();

	return newItems;
}

template<>
std::string AnyToString<RoadData*>(RoadData * const &d)
{
	std::stringstream sstr;

	sstr << "RoadData{" << std::fixed << d->getPoints() << "}";

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
