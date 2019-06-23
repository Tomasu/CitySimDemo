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

	 Point(float x, float y, float z = 0.0f);

	 float x() const { return mX; }
	 float y() const { return mY; }
	 float z() const { return mZ; }

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
	 Point operator+(const Point &other) const;

	 Point operator* (float factor) const;

	 Point operator* (const Point &other) const;

	 friend std::ostream & operator << (std::ostream &out, const Point &p);

	 friend inline const Point operator* (float factor, const Point &p);

	private:
		float mX;
		float mY;
		float mZ;
};

#include <ostream>
std::ostream &operator<<(std::ostream &out, const Point &p);

inline const Point operator* (float factor, const Point &p)
{
	return Point(p.mX * factor, p.mY * factor, p.mZ * factor);
}


#endif // POINT_H
