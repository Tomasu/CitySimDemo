#include "Plane3D.h"

#include <cmath>

#include "util/Rect.h"

// see:
// http://geomalgorithms.com/a04-_planes.html

Plane3D::Plane3D(const QVector3D& v0, const QVector3D& v1, const QVector3D& v2)
{
	// convert two points to origin and normal

	mOrigin = v0;

	// n = u x v = (v1 - v0) x (v2 - v0)
	mNormal = QVector3D::crossProduct(v1 - v0, v2 - v0);
}

Plane3D::Plane3D(const Rect& rect, float z)
{
	QVector3D v0 = QVector3D(rect.left(), rect.top(), z);
	QVector3D v1 = QVector3D(rect.right(), rect.top(), z);
	QVector3D v2 = QVector3D(rect.left(), rect.bottom(), z);

	mOrigin = v0;

	mNormal = QVector3D::crossProduct(v1 - v0, v2 - v0);
}


// see: http://geomalgorithms.com/a05-_intersect-1.html

#define SMALL_NUM   0.00000001 // anything that avoids division overflow

Plane3D::IntersectType Plane3D::intersects(const QVector3D& p1, const QVector3D& p2, QVector3D* intersection)
{
	// Let L: P(s) = P0 + s(P1 - P0) = P0 + su,

	QVector3D u = p2 - p1;
	QVector3D w = p1 - mOrigin;

	double D = QVector3D::dotProduct(mNormal, u);
	double N = -QVector3D::dotProduct(mNormal, w);

	if (fabs(D) < SMALL_NUM)
	{
		if (N == 0)
		{
			return IntersectInPlane; // line is entirely in plane
		}
		else
		{
			return IntersectDisjoint; // no intersection
		}
	}

	// not parallel

	double sI = N / D;

	if (sI < 0 || sI > 1)
	{
		return IntersectDisjoint; // no intersection
	}

	if (intersection != nullptr)
	{
		*intersection = p1 + sI * u;
	}

	return IntersectUnique;
}
