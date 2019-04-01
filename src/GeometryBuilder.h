//
// Created by moose on 29/03/19.
//

#ifndef CITYSIM_GEOMETRYBUILDER_H
#define CITYSIM_GEOMETRYBUILDER_H

#include <QtCore/QByteArray>
#include <ogr_geometry.h>
#include <QtGui/QVector4D>
#include <QtGui/QVector3D>
#include <QtGui/QPixmap>
#include "BufferBuilder.h"
#include "Constants.h"

// was going to have a fancy geometry builder class
// that could take commands to build various types of geometry and build a vertex buffer
//  I think it might be better to have a way to /build/ and /modify/ Qt3D Geometry/GeometryRenderer
//  objects rather than a bare vertex buffer builder like this was going to be...
class GeometryBuilder
{
	public:
		GeometryBuilder(double origX, double origY, int width, int height);
		~GeometryBuilder();

		void addLineString(const OGRLineString *lineString);
		void addPolygon(const OGRPolygon *polygon);

		void addLineString(const OGRLineString *lineString, const QVector4D &color);
		void addPolygon(const OGRPolygon *polygon, const QVector4D &color);

		uint32_t getNumVerticies();
		uint32_t getNumIndicies();

		QByteArray &getVerticies();
		QByteArray &getIndicies();

		QVector3D getMinPos() { return QVector3D(mMinX, mMinY, 0.0f); }
		QVector3D getMaxPos() { return QVector3D(mMaxX, mMaxY, 0.0f); }

		QPixmap *getPixmap() { return mPixmap; }

	private:
		uint32_t mNumVerticies;
		uint32_t mNumIndicies;

		BufferBuilder<VTX_TYPE> mVertexBuilder;
		BufferBuilder<IDX_TYPE> mIndexBuilder;

		double mOrigX;
		double mOrigY;

		double mMinX;
		double mMinY;
		double mMaxX;
		double mMaxY;

		void addPoint(const OGRPoint &point, const QVector4D &color);

		QPixmap *mPixmap;
		QPainter *mPainter;
};


#endif //CITYSIM_GEOMETRYBUILDER_H
