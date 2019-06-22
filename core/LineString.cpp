#include "LineString.h"

LineString::LineString(const std::vector<QVector3D> &points) : mPoints{points}
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



void LineString::add(const QVector3D& pt)
{
	mPoints.push_back(pt);
}

const QVector3D & LineString::operator[](int idx) const
{
	return mPoints.at(idx);
}


const std::vector<QVector3D> &LineString::getPoints() const
{
	return mPoints;
}


