#include "core/LineString.h"

LineString::LineString(const std::vector<Point> &points) : mPoints{points}
{
}

LineString::LineString()
{
}

LineString::LineString(const LineString& lineString)
	: mPoints{lineString.getPoints()}
{
}

void LineString::append(const LineString& lineString)
{
	auto &pts = lineString.getPoints();
	mPoints.insert(mPoints.end(), pts.begin(), pts.end());
}

void LineString::add(const Point& pt)
{
	mPoints.push_back(pt);
}

const Point & LineString::operator[](int idx) const
{
	return mPoints.at(idx);
}


const std::vector<Point> &LineString::getPoints() const
{
	return mPoints;
}


