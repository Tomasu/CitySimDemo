#include "util/Point.h"
#include "util/Math.h"

#include "util/LogUtils.h"
#define TAG "Point"

Point::Point()
	: mX{0.0f}, mY{0.0f}, mZ{0.0f}
{

}

Point::Point(const Point& other)
	: mX{other.mX}, mY{other.mY}, mZ{other.mZ}
{

}

Point::Point::Point(float x, float y, float z)
	: mX{x}, mY{y}, mZ{z}
{
}


Point& Point::operator=(const Point& other)
{
	mX = other.mX;
	mY = other.mY;
	mZ = other.mZ;
	return *this;
}

bool Point::operator==(const Point& other) const
{
	return fuzzyFloatCompare(mX, other.mX) && fuzzyFloatCompare(mY, other.mY) && fuzzyFloatCompare(mZ, other.mZ);
}

bool Point::operator!=(const Point& other) const
{
	return !fuzzyFloatCompare(mX, other.mX) || !fuzzyFloatCompare(mY, other.mY) || !fuzzyFloatCompare(mZ, other.mZ);
}

Point Point::operator-(const Point& other) const
{
	return Point(mX - other.mX, mY - other.mY, mZ - other.mZ);
}

Point Point::operator*(float mult) const
{
	return Point(mX * mult, mY * mult, mZ * mult);
}

Point Point::operator*(const Point& other) const
{
	return Point(mX * other.mX, mY * other.mY, mZ * other.mZ);
}

Point Point::operator+(const Point& other) const
{
	return Point(mX + other.mX, mY + other.mY, mZ + other.mZ);
}


#include <string>
#include <sstream>

template<>
std::string AnyToString<Point>(const Point &point)
{
	std::stringstream sstr;

	sstr << std::fixed << "Point{" << point.x() << "," << point.y() << "," << point.z() << "}";
	return sstr.str();
}

#include <ostream>
std::ostream &operator<<(std::ostream &out, const Point &p)
{
	out << AnyToString(p);
	return out;
}
