//
// Created by moose on 29/03/19.
//

#ifndef CITYSIM_GEOMETRYBUILDER_H
#define CITYSIM_GEOMETRYBUILDER_H

#include "core/Constants.h"

#include "util/Rect.h"
#include <stdint.h>

#include <vector>

class Point;
class QVector4D;
//class QByteArray;
#include <QByteArray>
#include <QColor>

class OGRPoint;
class OGRLineString;
class OGRPolygon;

class LineString;
class Line;

template<typename T>
class BufferBuilder;

// was going to have a fancy geometry builder class
// that could take commands to build various types of geometry and build a vertex buffer
//  I think it might be better to have a way to /build/ and /modify/ Qt3D Geometry/GeometryRenderer
//  objects rather than a bare vertex buffer builder like this was going to be...
class GeometryBuilder
{
	public:
		GeometryBuilder() = delete;
		explicit GeometryBuilder(const Rect &bounds, float z);
		GeometryBuilder(float origX, float origY, float origZ, int width, int height);
		~GeometryBuilder();

		void addLineString(const std::vector<Point> &lineString);
		void addPolygon(const std::vector<Point> &polygon);

		void addLine(const Line &line, const QColor &color);

		void addLineString(const LineString &lineString);
		void addLineString(const LineString &lineString, const QColor &color);

		void addLineString(const OGRLineString *lineString);
		void addPolygon(const OGRPolygon *polygon);

		void addLineString(const OGRLineString *lineString, const QColor &color);
		void addPolygon(const OGRPolygon *polygon, const QColor &color);

		uint32_t getNumVerticies();
		uint32_t getNumIndicies();

		const QByteArray &getVerticies();
		const QByteArray &getIndicies();

		Point getMinPos();
		Point getMaxPos();

		void setColor(const QColor &color);

		//QPixmap *getPixmap() { return mPixmap; }

	private:
		uint32_t mNumVerticies;
		uint32_t mNumIndicies;

		BufferBuilder<VTX_TYPE> *mVertexBuilder;
		BufferBuilder<IDX_TYPE> *mIndexBuilder;

		Rect mBounds;
		Point mOrigin;

		float mMinX;
		float mMinY;
		float mMaxX;
		float mMaxY;
		float mMinZ;
		float mMaxZ;

		QColor mCurrentColor;

// 		QPixmap *mPixmap;
// 		QPainter *mPainter;

		void addPoint(const OGRPoint &point, const QColor &color);

		void addPoint(const Point &point, const QColor &color);

      static LineString getCurvePoints(const LineString &controls, float detail);
      static Point cubicBezier(const Point &p1, const Point &p2, const Point &p3, const Point &p4, float t);
      static Point quadBezier(const Point &p1, const Point &p2, const Point &p3, float t);
      static float cubicBezierPoint(float a0, float a1, float a2, float a3, float t);
      static float quadBezierPoint(float a0, float a1, float a2, float t);
      static Point center(const Point &p1, const Point &p2);

		static int sId;
		int mId;
};


#endif //CITYSIM_GEOMETRYBUILDER_H
