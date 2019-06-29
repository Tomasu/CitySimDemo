#ifndef CITYSIM_LINE_H_GUARD
#define CITYSIM_LINE_H_GUARD

#include "util/Point.h"

class Line
{
	public:
		enum IntersectType {
			Disjoint = 0,
			Intersect,
			Overlap
		};

		Line(const Point &start, const Point &end);

		Point start() const;
		Point end() const;

		/**
		 * @param other other line
		 * @param I0 intersect point
		 * @param I1 end point of intersect segment in case of overlap (when it exists)
		 * @return IntersectType
		 */
		IntersectType intersect(const Line &other, Point *I0, Point *I1);

		bool contains(const Point &pt) const;

		float length() const;

	private:
		Point mStart;
		Point mEnd;
};

#endif /* CITYSIM_LINE_H_GUARD */
