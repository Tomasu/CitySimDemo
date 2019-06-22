#ifndef POINT_H
#define POINT_H

#include <ostream>

/**
 * @todo write docs
 */
class Point
{
public:
    /**
     * Default constructor
     */
    Point();

    /**
     * Copy constructor
     *
     * @param other TODO
     */
    Point(const Point& other);

	 Point(float x, float y);

	 float x() const { return mX; }
	 float y() const { return mY; }

    /**
     * Assignment operator
     *
     * @param other TODO
     * @return TODO
     */
    Point& operator=(const Point& other);

    /**
     * @todo write docs
     *
     * @param other TODO
     * @return TODO
     */
    bool operator==(const Point& other) const;

    /**
     * @todo write docs
     *
     * @param other TODO
     * @return TODO
     */
    bool operator!=(const Point& other) const;

	 Point operator-(const Point &other) const;

	 friend std::ostream & operator << (std::ostream &out, const Point &p);

	private:
		float mX;
		float mY;
};

#include <ostream>
std::ostream &operator<<(std::ostream &out, const Point &p);

#endif // POINT_H
