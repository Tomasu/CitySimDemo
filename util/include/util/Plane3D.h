#ifndef CITYSIM_PLANE_3D_H_GUARD
#define CITYSIM_PLANE_3D_H_GUARD

#include <QtGui/QVector3D>

class Rect;

class Plane3D
{
	public:
		enum IntersectType {
			IntersectDisjoint = 0,
			IntersectUnique,
			IntersectInPlane
		};

		Plane3D(const Rect &rect, float z = 0.0f);

		Plane3D(const QVector3D &v0, const QVector3D &v1, const QVector3D &v2);

		IntersectType intersects(const QVector3D &p1, const QVector3D &p2, QVector3D *intersection);

		QVector3D origin() const { return mOrigin; }
		QVector3D normal() const { return mNormal; }

	private:
		QVector3D mOrigin;
		QVector3D mNormal;
};

#endif /* CITYSIM_PLANE_3D_H_GUARD */
