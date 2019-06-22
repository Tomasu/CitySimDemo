#include "util/Rect.h"
#include "util/Math.h"

#include "Size.h"
#include <QVector3D>
#include <cmath>

#include "LogUtils.h"
#define TAG "Rect"

Rect::Rect()
	: mX{0.0f}, mY{0.0f}, mWidth{0.0f}, mHeight{0.0f}
{

}

Rect::Rect(const Point& origin, const Size& size)
	: mX{origin.x()}, mY{origin.y()}, mWidth{size.width()}, mHeight{size.height()}
{
}


Rect::Rect(const Rect& other)
	:mX{other.mX}, mY{other.mY}, mWidth{other.mWidth}, mHeight{other.mHeight}
{

}

Rect::Rect(float left, float top, float width, float height)
	:mX{left}, mY{top}, mWidth{width}, mHeight{height}
{
}

Rect::Rect(const Point& topLeft, const Point& bottomRight)
	: mX{topLeft.x()}, mY{topLeft.y()},
	mWidth{bottomRight.x() - topLeft.x()},
	mHeight{bottomRight.y() - topLeft.y()}
{

}

void Rect::moveTo(float x, float y)
{
	mX = x;
	mY = y;
}


bool Rect::isNull() const
{
	return fuzzyIsNull(mWidth) || fuzzyIsNull(mHeight);
}


bool Rect::contains(float x, float y) const
{
	if (mWidth <= FUZZY_COMPARE_DIFF || mHeight <= FUZZY_COMPARE_DIFF)
	{
		// null rect
		return false;
	}

	return x >= mX && x <= mX + mWidth && y >= mY && y <= mY + mHeight;
}

bool Rect::contains(const Point& vec) const
{
	return contains(vec.x(), vec.y());
}


bool Rect::contains(const QVector3D& vec) const
{
	return contains(vec.x(), vec.y());
}

bool Rect::intersects(const Rect& r) const
{
	float l1 = mX;
	float r1 = mX;

	if (mWidth < 0)
		l1 += mWidth;
	else
		r1 += mWidth;

	if (l1 == r1) // null rect
		return false;

	float l2 = r.mX;
	float r2 = r.mX;

	if (r.mWidth < 0)
		l2 += r.mWidth;
	else
		r2 += r.mWidth;

	if (l2 == r2) // null rect
		return false;

	if (l1 >= r2 || l2 >= r1)
		return false;

	float t1 = mY;
	float b1 = mY;

	if (mHeight < 0)
		t1 += mHeight;
	else
		b1 += mHeight;

	if (t1 == b1) // null rect
		return false;

	float t2 = r.mY;
	float b2 = r.mY;

	if (r.mHeight < 0)
		t2 += r.mHeight;
	else
		b2 += r.mHeight;

	if (t2 == b2) // null rect
		return false;

	if (t1 >= b2 || t2 >= b1)
		return false;

	return true;
}

bool Rect::contains(const Rect& r) const
{
	float l1 = mX;
	float r1 = mX;

	if (mWidth < 0)
		l1 += mWidth;
	else
		r1 += mWidth;

	if (l1 == r1) // null rect
		return false;

	float l2 = r.mX;
	float r2 = r.mX;

	if (r.mWidth < 0)
		l2 += r.mWidth;
	else
		r2 += r.mWidth;

	if (l2 == r2) // null rect
		return false;

	if (l2 < l1 || r2 > r1)
		return false;

	float t1 = mY;
	float b1 = mY;

	if (mHeight < 0)
		t1 += mHeight;
	else
		b1 += mHeight;

	if (t1 == b1) // null rect
		return false;

	float t2 = r.mY;
	float b2 = r.mY;

	if (r.mHeight < 0)
		t2 += r.mHeight;
	else
		b2 += r.mHeight;

	if (t2 == b2) // null rect
		return false;

	if (t2 < t1 || b2 > b1)
		return false;

	return true;
}

Point Rect::center() const
{
	return Point{mX + mWidth/2.0f, mY + mHeight/2.0f};
}


Rect& Rect::operator=(const Rect& other)
{
	mX = other.mX;
	mY = other.mY;
	mWidth = other.mWidth;
	mHeight = other.mHeight;

	return *this;
}

bool Rect::operator==(const Rect& other) const
{
	return fuzzyFloatCompare(mX, other.mX)
		&& fuzzyFloatCompare(mY, other.mY)
		&& fuzzyFloatCompare(mWidth, other.mWidth)
		&& fuzzyFloatCompare(mHeight, other.mHeight);
}

bool Rect::operator!=(const Rect& other) const
{
	return !fuzzyFloatCompare(mX, other.mX)
		|| !fuzzyFloatCompare(mY, other.mY)
		|| !fuzzyFloatCompare(mWidth, other.mWidth)
		|| !fuzzyFloatCompare(mHeight, other.mHeight);
}

void Rect::setHeight(float height)
{
	mHeight = height;
}

void Rect::setWidth(float width)
{
	mWidth = width;
}


#include <string>
#include <sstream>

std::ostream & operator << (std::ostream &out, const Rect &c)
{
	out << std::fixed << "Rect{" << c.mX << "," << c.mY << " " << c.mWidth << "x" << c.mHeight << "}";
	return out;
}

template<>
std::string AnyToString<Rect>(const Rect &rect)
{
	std::stringstream sstr;

	sstr << std::fixed << "Rect{" << rect.left() << "," << rect.top() << " " << rect.width() << "x" << rect.height() << "}";
	return sstr.str();
}
