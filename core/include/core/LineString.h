#ifndef CITYSIM_LINESTRING_H
#define CITYSIM_LINESTRING_H

#include <vector>

#include "util/Point.h"

class LineString
{
	public:
		LineString();
		LineString(const LineString &lineString);
		LineString(const std::vector<Point> &points);

		void add(const Point &pt);
		const std::vector<Point> &getPoints() const;

		void append(const LineString &lineString);

		size_t size() const { return mPoints.size(); }

		const Point &operator[] (int idx) const;

	private:
		std::vector<Point> mPoints;
};

#endif /* CITYSIM_LINESTRING_H */
