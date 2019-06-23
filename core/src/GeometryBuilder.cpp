//
// Created by moose on 29/03/19.
//

#include "core/GeometryBuilder.h"
#include "core/BufferBuilder.h"
#include "core/Line.h"
#include "core/LineString.h"


#include <iostream>
#include <iomanip>

#include <QDebug>
#include <QColor>
#include <QPainter>
#include <vector>
#include <QVector3D>
#include <QVector4D>

#include "util/Rect.h"

#include <ogr_geometry.h>


#include "util/LogUtils.h"

#define TAG "GeometryBuilder"

GeometryBuilder::GeometryBuilder(const Rect &bounds, float z)
	: mBounds{bounds}
{
	sId++;
	mId = sId;

	mVertexBuilder = new VertexBufferBuilder();
	mIndexBuilder = new IndexBufferBuilder();

	mNumVerticies = 0;
	mNumIndicies = 0;

	mOrigin = Point(bounds.left(), bounds.top(), z);

	log_debug("bz %s new gb @ %s [%s]", mId, mBounds, mOrigin);

	mMinX = mMaxX = mMinY = mMaxY = mMinZ = mMaxZ = 0.0f;

	mCurrentColor = QColor{0,0,0};
}

GeometryBuilder::GeometryBuilder(float origX, float origY, float origZ, int width, int height)
{
	sId++;
	mId = sId;

	mBounds = Rect(origX, origY, width, height);

	mVertexBuilder = new VertexBufferBuilder();
	mIndexBuilder = new IndexBufferBuilder();

	mNumVerticies = 0;
	mNumIndicies = 0;

	mOrigin = Point{origX, origY, origZ};

	log_debug("fi %s new gb @ %s [%s]", mId, mBounds, mOrigin);

	mMinX = mMaxX = mMinY = mMaxY = mMinZ = mMaxZ = 0.0f;

//	mPixmap = new QPixmap(width, height);
//	mPainter = new QPainter(mPixmap);
//
//	QPen pen(Qt::black, 1);
//	mPainter->setPen(pen);
//
//	QBrush brush(Qt::green);
//	mPainter->setBrush(brush);
//
//	mPainter->setClipping(true);
//	mPainter->setClipRect(QRect(0, 0, width, height));
//	mPainter->fillRect(0, 0, width, height, Qt::white);

}

GeometryBuilder::~GeometryBuilder()
{
	delete mVertexBuilder;
	delete mIndexBuilder;
	//delete mVerticies;
	//delete mIndexes;
}

void GeometryBuilder::setColor(const QColor& color)
{
	mCurrentColor = color;
}


void GeometryBuilder::addLineString(const OGRLineString *lineString)
{
	addLineString(lineString, mCurrentColor);
}

void GeometryBuilder::addPolygon(const OGRPolygon *polygon)
{
	addPolygon(polygon, mCurrentColor);
}

QVector3D ogrPointToQPoint(const OGRPoint &pt, const float origX, const float origY, const float origZ)
{
	return { std::round((float)pt.getX() - origX), std::round((float)pt.getY() - origY), std::round((float)pt.getZ() - origZ) };
}

Point GeometryBuilder::getMaxPos(){ return Point(mMaxX, mMaxY, 0.0f); }

Point GeometryBuilder::getMinPos(){ return Point(mMinX, mMinY, 0.0f); }

void GeometryBuilder::addLineString(const OGRLineString *lineString, const QColor &color)
{
	// TODO: break everything up into triangles, and allow wide lines...

	int numPoints = lineString->getNumPoints();

	if (numPoints < 2)
	{
		qDebug() << " ERROR: MUST HAVE AT LEAST 2 POINTS: got " << numPoints;
	}

	//mVerticies->resize(mVerticies->size() + sizeof(float) * numPoints * 6);

	int origVertOff = mNumVerticies;

	//qDebug() << "new line";

//	std::vector<QVector3D> qpoints;

	int addedpoints = 0;
	for (int i = 0; i < numPoints-1; i++)
	{
		OGRPoint pt; lineString->getPoint(i, &pt);
		OGRPoint pt2; lineString->getPoint(i+1, &pt2);

		addPoint(pt, color);
		addPoint(pt2, color);

//		QVector3D qpt = ogrPointToQPoint(pt, mOrigX, mOrigY);
//		qpoints.append(qpt);

		//std::cout << std::setprecision(10) << " pt: " << pt.getX() << ", " << pt.getY() << std::endl;


		addedpoints++;
	}

//	if (addedpoints != numPoints)
//	{
//		qDebug() << "addLineString: didn't add correct number of points. got " << addedpoints << ". wanted " << numPoints;
//	}

//	for (int i = 0; i < numPoints; i++)
//	{
//		mIndexBuilder.add(origVertOff + i);
//		//mIndexBuilder.add(origVertOff + i + 1);
//		mNumIndicies += 1;
//	}

	//mIndexBuilder.add(VTX_IDX_RESTART_ID);
	//mNumIndicies++;

//	mPainter->drawPolyline(qpoints.data(), qpoints.size());
}

void GeometryBuilder::addPolygon(const OGRPolygon *polygon, const QColor &color)
{
	//polygon->GetGeometryRef
	const OGRLinearRing *ext_ring = polygon->getExteriorRing();
	if (ext_ring == nullptr)
	{
		log_error("polygon with no ext ring?");
		return;
	}

	addLineString(ext_ring, color);

	int numIntRings = polygon->getNumInteriorRings();

	for (int i = 0; i < numIntRings; ++i)
	{
		const OGRLinearRing *intRing = polygon->getInteriorRing(i);
		addLineString(intRing, color);
	}
}

uint32_t GeometryBuilder::getNumVerticies()
{
	return mNumVerticies;
}

uint32_t GeometryBuilder::getNumIndicies()
{
	return mNumIndicies;
}

const QByteArray &GeometryBuilder::getVerticies()
{
	return mVertexBuilder->byteArray();
}

const QByteArray &GeometryBuilder::getIndicies()
{
	return mIndexBuilder->byteArray();
}

#include <stdexcept>
void GeometryBuilder::addPoint(const OGRPoint &point, const QColor &color)
{
	throw std::runtime_error("blah");

	float x = point.getX();// - mOrigX;
	float y = point.getY();// - mOrigY;

	if (x < mMinX)
	{
		mMinX = x;
	}

	if (x > mMaxX)
	{
		mMaxX = x;
	}

	if (y < mMinY)
	{
		mMinY = y;
	}

	if (y > mMaxY)
	{
		mMaxY = y;
	}

//		if (x < 0 || y < 0 || x > 40000 || y > 40000)
//		{
//			std::cout << std::setprecision(10) << " pt: " << x << ", " << y <<  " orig: " << pt.getX() << ", " << pt.getY() << std::endl;
//		}

	//std::cout << std::setprecision(10) << " pt: " << x << ", " << y << std::endl;

	// position
	mVertexBuilder->add(x);
	mVertexBuilder->add(y);
	mVertexBuilder->add(0.0f);

	// normal
// 	mVertexBuilder->add(0.0f);
// 	mVertexBuilder->add(0.0f);
// 	mVertexBuilder->add(0.0f);

	// color
	mVertexBuilder->add(color.red());
	mVertexBuilder->add(color.green());
	mVertexBuilder->add(color.blue());
	mVertexBuilder->add(color.alpha());

	//mIndexBuilder.add(mNumVerticies);

	//mNumIndicies++;

	mNumVerticies++;
}

void GeometryBuilder::addLine(const Line &line, const QColor &color)
{
	//log_debug("%s s=%s, e=%s [%s]", mId, line.start(), line.end(), mOrigin);
	addPoint(line.start(), color);
	addPoint(line.end(), color);
}

void GeometryBuilder::addLineString(const LineString& lineString)
{
	addLineString(lineString, mCurrentColor);
}

void GeometryBuilder::addLineString(const LineString& lineString, const QColor& color)
{
	for (const Point &pt: lineString.getPoints())
	{
		addPoint(pt, color);
	}

// 	if (lineString.size() < 3)
// 	{
// 		for (const QVector3D &pt: lineString.getPoints())
// 		{
// 			addPoint(pt, color);
// 		}
// 		return;
// 	}
//
// 	LineString copy;
// 	copy.add(lineString[0]);
// 	copy.append(lineString);
// 	copy.add(lineString[lineString.size()-1]);
//
// 	LineString interpolated = getCurvePoints(copy, 0.1);
// 	std::vector<QVector3D> points = interpolated.getPoints();
//// 	for (const QVector3D &pt: points)
//// 	{
//// 		addPoint(pt, color);
//// 	}
// 	int numPoints = points.size();
// 	for (int i = 0; i < numPoints-1; i++)
// 	{
// 		QVector3D p1 = points[i];
// 		QVector3D p2 = points[i+1];
// 		addPoint(p1, color);
// 		addPoint(p2, color);
// 	}

	// missing end point??
	//addPoint(lineString[lineString.size()-1], color);
}


void GeometryBuilder::addLineString(const std::vector<Point> &lineString)
{
	addLineString(lineString, mCurrentColor);
}

void GeometryBuilder::addPolygon(const std::vector<Point> &polygon)
{

}

void GeometryBuilder::addPoint(const Point &p, const QColor &color)
{
	Point poff = p - mOrigin;
	float x = poff.x();
	float y = poff.y();
	float z = poff.z();

	//log_debug("%s pt=%sx%sx%s (%sx%sx%s) [%s]", mId, x, y, z, p.x(), p.y(), p.z(), mOrigin);

	if (x < mMinX)
	{
		mMinX = x;
	}

	if (x > mMaxX)
	{
		mMaxX = x;
	}

	if (y < mMinY)
	{
		mMinY = y;
	}

	if (y > mMaxY)
	{
		mMaxY = y;
	}

	if (z < mMinZ)
	{
		mMinZ = z;
	}

	if (z > mMaxZ)
	{
		mMaxZ = z;
	}

	// position
	mVertexBuilder->add(x);
	mVertexBuilder->add(y);
	mVertexBuilder->add(z);

	// normal
	// mVertexBuilder->add(1.0f);
	// mVertexBuilder->add(1.0f);
	// mVertexBuilder->add(1.0f);

	// color
	mVertexBuilder->add(color.redF());
	mVertexBuilder->add(color.greenF());
	mVertexBuilder->add(color.blueF());
	mVertexBuilder->add(color.alphaF());

	//mIndexBuilder.add(mNumVerticies);

	//mNumIndicies++;

	mNumVerticies++;
}

/**
 *
 * Generates several 3D points in a continuous Bezier curve based upon
 * the parameter list of points.
 *
 * @param controls
 * @param detail
 * @return
 */

LineString GeometryBuilder::getCurvePoints(const LineString &controls, float detail)
{
	if ( detail > 1 || detail < 0 ){
		throw std::runtime_error("illegal argument");
	}

	LineString renderingPoints;

	std::vector<Point> controlPoints;

	//generate the end and control points

	for ( size_t i = 1; i < controls.size() - 1; i+=2 )
	{
		controlPoints.push_back(center(controls[i-1], controls[i]));
		controlPoints.push_back(controls[i]);
		controlPoints.push_back(controls[i+1]);

		if ( i+2 < controls.size()- 1 )
		{
			controlPoints.push_back(center(controls[i+1], controls[i+2]));
		}
	}

	//Generate the detailed points.

	Point a0, a1, a2, a3;

	for ( size_t i = 0; i < controlPoints.size() - 2; i+=4 )
	{
		a0 = controlPoints.at(i);
		a1 = controlPoints.at(i+1);
		a2 = controlPoints.at(i+2);

		if ( i + 3 > controlPoints.size() - 1 )
		{
			//quad

			for ( float j = 0; j < 1; j += detail)
			{
				renderingPoints.add(quadBezier(a0,a1,a2,j));
			}
		}
		else
		{
			//cubic

			a3 = controlPoints.at(i+3);

			for ( float j = 0; j < 1; j += detail)
			{
				renderingPoints.add(cubicBezier(a0,a1,a2,a3,j));
			}
		}
	}

	return renderingPoints;
}



/**
 *
 * A cubic bezier method to calculate the point at t along the Bezier Curve give
 *
 * the parameter points.
 *
 * @param p1
 * @param p2
 * @param p3
 * @param p4
 * @param t A value between 0 and 1, inclusive.
 * @return
 */

Point GeometryBuilder::cubicBezier(const Point &p1, const Point &p2, const Point &p3, const Point &p4, float t)
{
	return Point(
		cubicBezierPoint(p1.x(), p2.x(), p3.x(), p4.x(), t),
							 cubicBezierPoint(p1.y(), p2.y(), p3.y(), p4.y(), t),
							 cubicBezierPoint(p1.z(), p2.z(), p3.z(), p4.z(), t));

}



/**
 *
 * A quadratic Bezier method to calculate the point at t along the Bezier Curve give
 *
 * the parameter points.
 *
 * @param p1
 * @param p2
 * @param p3
 * @param t A value between 0 and 1, inclusive.
 * @return
 *
 */

Point GeometryBuilder::quadBezier(const Point &p1, const Point &p2, const Point &p3, float t)
{
	return Point(
		quadBezierPoint(p1.x(), p2.x(), p3.x(), t),
							 quadBezierPoint(p1.y(), p2.y(), p3.y(), t),
							 quadBezierPoint(p1.z(), p2.z(), p3.z(), t));

}

/**
 *
 * The cubic Bezier equation.
 *
 * @param a0
 * @param a1
 * @param a2
 * @param a3
 * @param t
 * @return
 */

float GeometryBuilder::cubicBezierPoint(float a0, float a1, float a2, float a3, float t)
{
	return pow(1-t, 3) * a0 + 3* pow(1-t, 2) * t * a1 + 3*(1-t) * pow(t, 2) * a2 + pow(t, 3) * a3;
}



/**
 *
 * The quadratic Bezier equation,
 *
 * @param a0
 * @param a1
 * @param a2
 * @param t
 * @return
 */

float GeometryBuilder::quadBezierPoint(float a0, float a1, float a2, float t)
{
	return pow(1-t, 2) * a0 + 2* (1-t) * t * a1 + pow(t, 2) * a2;
}



/**
 *
 * Calculates the center point between the two parameter points.
 *
 * @param p1
 * @param p2
 * @return
 */

Point GeometryBuilder::center(const Point &p1, const Point &p2)
{
	return Point(
		(p1.x() + p2.x()) / 2,
			  (p1.y() + p2.y()) / 2,
			  (p1.z() + p2.z()) / 2
	);
}

int GeometryBuilder::sId = 0;
