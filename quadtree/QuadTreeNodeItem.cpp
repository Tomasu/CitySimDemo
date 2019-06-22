//
// Created by moose on 04/04/19.
//

#include "QuadTreeNodeItem.h"

#include "LogUtils.h"
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
