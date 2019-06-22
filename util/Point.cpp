#include "util/Point.h"
#include "util/Math.h"

#include "LogUtils.h"
#define TAG "Point"

Point::Point()
	: mX{0.0f}, mY{0.0f}
{

}

Point::Point(const Point& other)
	: mX{other.mX}, mY{other.mY}
{

}

Point::Point::Point(float x, float y)
	: mX{x}, mY{y}
{
}


Point& Point::operator=(const Point& other)
{
	mX = other.mX;
	mY = other.mY;
	return *this;
}

bool Point::operator==(const Point& other) const
{
	return fuzzyFloatCompare(mX, other.mX) && fuzzyFloatCompare(mY, other.mY);
}

bool Point::operator!=(const Point& other) const
{
	return !fuzzyFloatCompare(mX, other.mX) || !fuzzyFloatCompare(mY, other.mY);
}

Point Point::operator-(const Point& other) const
{
	return Point(mX - other.mX, mY - other.mY);
}


#include <string>
#include <sstream>

template<>
std::string AnyToString<Point>(const Point &point)
{
	std::stringstream sstr;

	sstr << std::fixed << "Point{" << point.x() << "," << point.y() << "}";
	return sstr.str();
}

#include <ostream>
std::ostream &operator<<(std::ostream &out, const Point &p)
{
	out << AnyToString(p);
	return out;
}
