#ifndef QUADTREENODEPOINTITEM_H
#define QUADTREENODEPOINTITEM_H

#include "QuadTreeNodeItem.h"

#include <string>

/**
 * @todo write docs
 */
class QuadTreeNodePointItem : public QuadTreeNodeItem
{
	public:
		explicit QuadTreeNodePointItem() = delete;
		explicit QuadTreeNodePointItem(const std::string &name, const Rect &bounds);
		QuadTreeNodePointItem(const std::string &name, const Rect& bounds, const std::vector< Point >& points);
		~QuadTreeNodePointItem() = default;

    /**
     * @todo write docs
     *
     * @return TODO
     */
    virtual std::string toString() override;

    /**
     * @todo write docs
     *
     * @param rect TODO
     * @return TODO
     */
    virtual bool isContainedByRect(const Rect& rect) const override;

    /**
     * @todo write docs
     *
     * @param rect TODO
     * @return TODO
     */
    virtual bool isPartiallyContainedByRect(const Rect& rect) const override;

    /**
     * @todo write docs
     *
     * @param bounds TODO
     * @param crossPoint TODO
     * @return TODO
     */
    virtual std::vector< QuadTreeNodeItem* > split(const Rect& bounds, const Point& crossPoint) override;

    /**
     * @todo write docs
     *
     * @param gb TODO
     * @return TODO
     */
    virtual void buildGeometry(GeometryBuilder* gb) override;

	 virtual void addSegment(const Point &start, const Point &end);

	 size_t getNumPoints() const { return mPoints.size(); }
	 const std::vector<Point> &getPoints() const { return mPoints; }

	 const Point &pointAt(int idx) const { return mPoints[idx]; }

	 float length() const override;

	private:
		std::vector<Point> mPoints;

};

#endif // QUADTREENODEPOINTITEM_H
