#ifndef CITYSIM_LINE_H_GUARD
#define CITYSIM_LINE_H_GUARD

#include <QtGui/QVector3D>
class QVector3D;
class Point;

class Line
{
	public:
		enum IntersectType {
			Disjoint = 0,
			Intersect,
			Overlap
		};

		Line(const QVector3D &start, const QVector3D &end);
		Line(const Point &start, const Point &end);

		QVector3D start() const;
		QVector3D end() const;

		/**
		 * @param other other line
		 * @param I0 intersect point
		 * @param I1 end point of intersect segment in case of overlap (when it exists)
		 * @return IntersectType
		 */
		IntersectType intersect(const Line &other, QVector3D *I0, QVector3D *I1);

		bool contains(const QVector3D &pt) const;

	private:
		QVector3D mStart;
		QVector3D mEnd;
};

#endif /* CITYSIM_LINE_H_GUARD */
