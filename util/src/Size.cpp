#include "util/Size.h"
#include "util/Math.h"

Size::Size()
	: mWidth{0.0f}, mHeight{0.0f}
{

}

Size::Size(float width, float height)
	: mWidth{width}, mHeight{height}
{
}


Size::Size(const Size& other)
	: mWidth{other.mWidth}, mHeight{other.mHeight}
{

}

Size& Size::operator=(const Size& other)
{
	mWidth = other.mWidth;
	mHeight = other.mHeight;
	return *this;
}

bool Size::operator==(const Size& other) const
{
	return fuzzyFloatCompare(mWidth, other.mWidth)
		&& fuzzyFloatCompare(mHeight, other.mHeight);
}

bool Size::operator!=(const Size& other) const
{
	return !fuzzyFloatCompare(mWidth, other.mWidth)
		|| !fuzzyFloatCompare(mHeight, other.mHeight);
}

Size Size::operator/(int div) const
{
	return Size(mWidth / div, mHeight / div);
}

