#ifndef RECT_H
#define RECT_H

#include "util/Size.h"
#include "util/Point.h"

#include <ostream>

/**
 * @todo write docs
 */
class Rect
{
	public:
		/**
		* Default constructor
		*/
		Rect();

		Rect(float left, float top, float width, float height);
		Rect(const Point &origin, const Size &size);

		Rect(const Point &topLeft, const Point &bottomRight);

		/**
		* Copy constructor
		*
		* @param other TODO
		*/
		Rect(const Rect& other);

		/**
		* Assignment operator
		*
		* @param other TODO
		* @return TODO
		*/
		Rect& operator=(const Rect& other);

		/**
		* @todo write docs
		*
		* @param other TODO
		* @return TODO
		*/
		bool operator==(const Rect& other) const;

		/**
		* @todo write docs
		*
		* @param other TODO
		* @return TODO
		*/
		bool operator!=(const Rect& other) const;

		float left() const { return mX; }
		float top() const { return mY; }
		float width() const { return mWidth; }
		float height() const { return mHeight; }

		float right() const { return mX + mWidth; }
		float bottom() const { return mY + mHeight; }

		Point center() const;

		Point topLeft() const { return Point(mX, mY); }
		Point topRight() const { return Point(mX + mWidth, mY); }
		Point bottomLeft() const { return Point(mX, mY + mHeight); }
		Point bottomRight() const { return Point(mX + mWidth, mY + mHeight); }

		Size size() const { return Size(mWidth, mHeight); }

		void adjustSize(float width, float height);

		Rect sizeAdjusted(float width, float height) const;

		void setWidth(float width);
		void setHeight(float height);

		void moveTo(float x, float y);

		bool contains(float x, float y) const;
		bool contains(const Point &vec) const;
		bool contains(const Rect &r) const;

		bool intersects(const Rect& rect) const;

		bool isNull() const;

		friend std::ostream & operator << (std::ostream &out, const Rect &c);

	private:
		float mX;
		float mY;
		float mWidth;
		float mHeight;
};

#endif // RECT_H
