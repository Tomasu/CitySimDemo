#include "core/Line.h"

#include <QtGui/QVector3D>

#include "util/Point.h"

#include "util/LogUtils.h"
#include "core/LogUtilsQt.h"
#define TAG "Line"

Line::Line(const Point& start, const Point& end)
	: mStart{start},
	mEnd{end}
{
}


Point Line::start() const
{
	return mStart;
}

Point Line::end() const
{
	return mEnd;
}

#include <cmath>
/* perp product  (2D) */
#define perp(u,v)  ((u).x() * (v).y() - (u).y() * (v).x())
#define dot(u,v)   ((u).x() * (v).x() + (u).y() * (v).y() + (u).z() * (v).z())

#define SMALL_NUM   0.00000001 // anything that avoids division overflow

// inSegment(): determine if a point is inside a segment
//    Input:  a point P, and a collinear segment S
//    Return: 1 = P is inside S
//            0 = P is  not inside S
bool Line::contains(const Point &pt) const
{
	if (mStart.x() != mEnd.x())
	{
		// S is not  vertical
		if (mStart.x() <= pt.x() && pt.x() <= mEnd.x())
		{
			return true;
		}

		if (mStart.x() >= pt.x() && pt.x() >= mEnd.x())
		{
			return true;
		}
	}
	else
	{
		// S is vertical, so test y  coordinate
		if (mStart.y() <= pt.y() && pt.y() <= mEnd.y())
		{
			return true;
		}

		if (mStart.y() >= pt.y() && pt.y() >= mEnd.y())
		{
			return true;
		}
	}

	return false;
}

Line::IntersectType Line::intersect(const Line& other, Point* I0, Point* I1)
{
	Point u = mEnd - mStart;
	Point v = other.mEnd - other.mStart;
	Point w = mStart - other.mStart;
	float D = perp(u,v);

	// test if  they are parallel (includes either being a point)
	if (fabs(D) < SMALL_NUM)
	{
		// S1 and S2 are parallel
		if (perp(u,w) != 0 || perp(v,w) != 0)
		{
			// they are NOT collinear
			return Disjoint;
		}

		// they are collinear or degenerate
		// check if they are degenerate  points
		float du = dot(u,u);
		float dv = dot(v,v);

		if (du==0 && dv==0)
		{
			// both segments are points
			if (mStart !=  other.mStart)
			{
				// they are distinct  points
				return Disjoint;
			}

			// they are the same point
			*I0 = mStart;
			return Intersect;
		}

		if (du==0)
		{
			// S1 is a single point
			if  (!other.contains(mStart) == 0)  // but is not in S2
			{
				return Disjoint;
			}

			*I0 = mStart;
			return Intersect;
		}

		if (dv==0)
		{
			// S2 a single point
			if  (!this->contains(other.mStart))  // but is not in S1
			{
				return Disjoint;
			}

			*I0 = other.mStart;
			return Intersect;
		}

		// they are collinear segments - get  overlap (or not)
		float t0, t1; // endpoints of S1 in eqn for S2
		Point w2 = mEnd - other.mStart;
		if (v.x() != 0)
		{
			t0 = w.x() / v.x();
			t1 = w2.x() / v.x();
		}
		else
		{
			t0 = w.y() / v.y();
			t1 = w2.y() / v.y();
		}

		if (t0 > t1)
		{
			// must have t0 smaller than t1
			float t=t0;

			// swap if not
			t0=t1;
			t1=t;
		}

		if (t0 > 1 || t1 < 0)
		{
			return Disjoint;      // NO overlap
		}

		t0 = t0<0? 0 : t0;               // clip to min 0
		t1 = t1>1? 1 : t1;               // clip to max 1

		if (t0 == t1)
		{
			// intersect is a point
			*I0 = other.mStart +  t0 * v;
			return Intersect;
		}

		// they overlap in a valid subsegment
		*I0 = other.mStart + t0 * v;
		*I1 = other.mStart + t1 * v;

		return Overlap;
	}

	// the segments are skew and may intersect in a point
	// get the intersect parameter for S1
	float     sI = perp(v,w) / D;
	if (sI < 0 || sI > 1)
	{
		// no intersect with S1
		return Disjoint;
	}

	// get the intersect parameter for S2
	float     tI = perp(u,w) / D;
	if (tI < 0 || tI > 1)
	{
		// no intersect with S2
		return Disjoint;
	}

	*I0 = mStart + sI * u;                // compute S1 intersect point

	return Intersect;
}

float Line::length() const
{
	Point diff = mEnd - mStart;

	return sqrtf(diff.x() * diff.x() + diff.y() * diff.y() + diff.z() * diff.z());
}


#include <ostream>
#include <iomanip>

std::ostream & operator << (std::ostream &out, const Line &c)
{
	out << std::fixed << "Line{" << c.start() << "," << c.end() << "}";
	return out;
}

#include <sstream>
#include <iomanip>
template<>
std::string AnyToString<Line>(const Line &d)
{
	std::stringstream sstr;

	sstr <<  std::fixed << "Line{" << d.start() << ", " << d.end() << "}";

	return sstr.str();
}

template<>
std::string AnyToString<Line::IntersectType>(const Line::IntersectType &d)
{
	std::stringstream sstr;

	std::string str;
	switch(d)
	{
		case Line::Intersect:
			str = "Intersect";
			break;

		case Line::Overlap:
			str = "Overlap";
			break;

		case Line::Disjoint:
			str = "Disjoint";
			break;

		default:
			str = "UNKNOWN";
			break;
	}

	return str;
}

