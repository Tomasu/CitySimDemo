#ifndef SIZE_H
#define SIZE_H

/**
 * @todo write docs
 */
class Size
{
	public:
    /**
     * Default constructor
     */
    Size();

    /**
     * Copy constructor
     *
     * @param other TODO
     */
    Size(const Size& other);

	 Size(float width, float height);

    /**
     * Assignment operator
     *
     * @param other TODO
     * @return TODO
     */
    Size& operator=(const Size& other);

    /**
     * @todo write docs
     *
     * @param other TODO
     * @return TODO
     */
    bool operator==(const Size& other) const;

    /**
     * @todo write docs
     *
     * @param other TODO
     * @return TODO
     */
    bool operator!=(const Size& other) const;

	 Size operator/(int div) const;

	 float width() const { return mWidth; }
	 float height() const { return mHeight; }

	private:
		float mWidth;
		float mHeight;
};

#endif // SIZE_H
