//
// Created by moose on 04/04/19.
//

#include "quadtree/QuadTreeNodeItem.h"

#include "util/LogUtils.h"
#define TAG "QuadTreeNodeItem"

template<>
std::string AnyToString<QuadTreeNodeItem*>(QuadTreeNodeItem *const &d)
{
	return d->toString();
}

#include <ostream>
std::ostream & operator << (std::ostream &out, QuadTreeNodeItem * const &d)
{
	out << AnyToString(d);

	return out;
}

#include <sstream>

template<> std::string AnyToString(const std::vector<QuadTreeNodeItem*> &d)
{
	std::stringstream sstr;

	bool haveItem = false;
	for (auto &item: d)
	{
		if (haveItem)
		{
			sstr << ", ";
		}

		sstr << item->toString();

		haveItem = true;
	}

	return sstr.str();
}
