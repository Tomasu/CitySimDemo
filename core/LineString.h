#ifndef CITYSIM_LINESTRING_H
#define CITYSIM_LINESTRING_H

#include <vector>
#include <QVector3D>

class LineString
{
	public:
		LineString();
		LineString(const LineString &lineString);
		LineString(const std::vector<QVector3D> &points);

		void add(const QVector3D &pt);
		const std::vector<QVector3D> &getPoints() const;

		void append(const LineString &lineString);

		size_t size() const { return mPoints.size(); }

		const QVector3D &operator[] (int idx) const;

	private:
		std::vector<QVector3D> mPoints;
};

#endif /* CITYSIM_LINESTRING_H */
