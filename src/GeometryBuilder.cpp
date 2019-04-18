//
// Created by moose on 29/03/19.
//

#include "GeometryBuilder.h"
#include "LogUtils.h"
#include <QDebug>
#include <iostream>
#include <iomanip>
#include <QColor>
#include <QPainter>

#define TAG "GeometryBuilder"

GeometryBuilder::GeometryBuilder(const QRectF &bounds)
{
	mNumVerticies = 0;
	mNumIndicies = 0;

	mOrigX = bounds.x();
	mOrigY = bounds.y();

	mMinX = mMaxX = mMinY = mMaxY = 0.0f;
}

GeometryBuilder::GeometryBuilder(double origX, double origY, int width, int height)
{
	mNumVerticies = 0;
	mNumIndicies = 0;
	mOrigX = origX;
	mOrigY = origY;
	mMinX = mMaxX = mMinY = mMaxY = 0.0f;

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
	//delete mVerticies;
	//delete mIndexes;
}

void GeometryBuilder::addLineString(const OGRLineString *lineString)
{
	addLineString(lineString, QVector4D(0.0, 0.5, 0.5, 0.8));
}

void GeometryBuilder::addPolygon(const OGRPolygon *polygon)
{
	addPolygon(polygon, QVector4D(0.5, 0.5, 0.0, 0.5));
}

QPointF ogrPointToQPoint(const OGRPoint &pt, const double origX, const double origY)
{
	return { std::round(pt.getX() - origX), std::round(pt.getY() - origY) };
}

void GeometryBuilder::addLineString(const OGRLineString *lineString, const QVector4D &color)
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

//	QVector<QPointF> qpoints;

	int addedpoints = 0;
	for (int i = 0; i < numPoints-1; i++)
	{
		OGRPoint pt; lineString->getPoint(i, &pt);
		OGRPoint pt2; lineString->getPoint(i+1, &pt2);

		addPoint(pt, color);
		addPoint(pt2, color);

//		QPointF qpt = ogrPointToQPoint(pt, mOrigX, mOrigY);
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

void GeometryBuilder::addPolygon(const OGRPolygon *polygon, const QVector4D &color)
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
	return mVertexBuilder.byteArray();
}

const QByteArray &GeometryBuilder::getIndicies()
{
	return mIndexBuilder.byteArray();
}

void GeometryBuilder::addPoint(const OGRPoint &point, const QVector4D &color)
{
	double x = point.getX() - mOrigX;
	double y = point.getY() - mOrigY;

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

	mVertexBuilder.add(x);
	mVertexBuilder.add(y);
	mVertexBuilder.add(0.0f);

	mVertexBuilder.add(color.x());
	mVertexBuilder.add(color.y());
	mVertexBuilder.add(color.z());
	mVertexBuilder.add(color.w());

	//mIndexBuilder.add(mNumVerticies);

	//mNumIndicies++;

	mNumVerticies++;
}

void GeometryBuilder::addLineString(const QVector<QPointF> &lineString)
{
	addLineString(lineString, QVector4D(0.0, 0.5, 0.5, 0.8));
}

void GeometryBuilder::addLineString(const QVector<QPointF> &lineString, const QVector4D &color)
{
	for (int i = 0; i < lineString.size(); ++i)
	{
		addPoint(lineString.at(i), color);
	}
}

void GeometryBuilder::addPolygon(const QVector<QPointF> &polygon)
{

}

void GeometryBuilder::addPolygon(const QVector<QPointF> &polygon, const QVector4D &color)
{

}

void GeometryBuilder::addPoint(const QPointF &point, const QVector4D &color)
{
	double x = point.x() - mOrigX;
	double y = point.x() - mOrigY;

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

	mVertexBuilder.add(x);
	mVertexBuilder.add(y);
	mVertexBuilder.add(0.0f);

	mVertexBuilder.add(color.x());
	mVertexBuilder.add(color.y());
	mVertexBuilder.add(color.z());
	mVertexBuilder.add(color.w());

	//mIndexBuilder.add(mNumVerticies);

	//mNumIndicies++;

	mNumVerticies++;
}


