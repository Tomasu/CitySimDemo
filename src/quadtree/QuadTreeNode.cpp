#include <memory>
#include <src/LogUtils.h>

//
// Created by moose on 04/04/19.
//

#include "QuadTreeNode.h"
#include "QuadTreeNodeItem.h"
#include "QuadTreeNodeEntity.h"
#include "QuadTreeNodeEntityFactory.h"
#include "QuadTreeNodeGeometry.h"
#include "QuadTreeNodeGeometryRenderer.h"

#define TAG "QuadTreeNode"

QuadTreeNode::QuadTreeNode(QuadTreeNodeEntityFactory *entityFactory, const QRectF &bounds)
		: mBounds{bounds}, mHaveSubNodes{false}, mEntityFactory(entityFactory), mEntity{nullptr}
{
	mGeometry = nullptr;
}

QuadTreeNode::~QuadTreeNode()
{
	std::scoped_lock<std::mutex> lock(mMutex);

	if (mHaveSubNodes)
	{
		// ensure our subnode references get cleared..
		// TODO: not sure this is needed, but meh
		for (auto &subNode: mSubNodes)
		{
			delete subNode;
		}

		mSubNodes.clear();
	}

	for (auto &item: mItems)
	{
		delete item;
	}

	mItems.clear();

	// if we change mItems to a ptr, delete it here
}

bool QuadTreeNode::insertItem(QuadTreeNodeItem *item, bool noupdate)
{
	// may want to make this lock a little more granular later?
	//std::scoped_lock<std::mutex> lock(mMutex);

	const QRectF &bounds = item->bounds();
// if item's center is inside this rect
	if (mBounds.contains(bounds.center()))
	{
		if (!mHaveSubNodes && mItems.size() >= NODE_SPLIT_COUNT)
		{
			splitNode();
		}

		if (mHaveSubNodes)
		{
			for (auto &subNode: mSubNodes)
			{
				if (subNode->insertItem(item, noupdate))
				{
					return true;
				}
			}
		}
		else
		{
			mItems.push_back(item);
			if (!noupdate)
			{
				updateGeometry();
			}
			return true;
		}
	}

	return false;
}

void QuadTreeNode::updateGeometry()
{
	log_debug("bounds: %s", mBounds);

	if (mEntity != nullptr)
	{
		//log_debug("update geom here!");
		mEntity->renderer()->setGeometryFactory(mEntity->renderer()->geometryFactory());
	}
	else
	{
		mEntity = (*mEntityFactory)(this);
	}

}

void QuadTreeNode::removeItem(QuadTreeNodeItem *)
{
	// TODO: implement me!
	throw std::runtime_error("unimplemented!");
}

Qt3DRender::QGeometry *QuadTreeNode::createGeometry()
{
	log_trace_enter();

	if (mGeometry != nullptr)
	{
		delete mGeometry;
	}

	mGeometry = new QuadTreeNodeGeometry(this);

	log_trace_exit();

	return mGeometry;
}

void QuadTreeNode::splitNode()
{
	log_debug("haveSubNodes: %s", mHaveSubNodes ? "true" : "false");

	// initial implementation will just hold the lock during the entire process
	//  it is possible we could take a copy of the data and mark this node as read-only
	//  till the process is done and swap in the new data after. and only hold locks
	//  during the initial copy and final copy.
	if (!mHaveSubNodes)
	{
		//std::scoped_lock<std::mutex> lock(mMutex);

		mHaveSubNodes = true;

		log_debug("split node: %s begin: numItems=%s", mBounds, mItems.size());

		QSizeF size = mBounds.size() / 2;

		QRectF topLeft{ mBounds.topLeft(), size };
		QRectF topRight{ {mBounds.center().x(), mBounds.top()}, size };
		QRectF bottomLeft{ {mBounds.left(), mBounds.center().y()}, size };
		QRectF bottomRight{ mBounds.center(), size };

		log_debug("new quads: %s %s %s %s", topLeft, topRight, bottomLeft, bottomRight);

		mSubNodes.push_back(new QuadTreeNode(mEntityFactory, topLeft));
		mSubNodes.push_back(new QuadTreeNode(mEntityFactory, topRight));
		mSubNodes.push_back(new QuadTreeNode(mEntityFactory, bottomLeft));
		mSubNodes.push_back(new QuadTreeNode(mEntityFactory, bottomRight));

		auto it = mItems.begin();
		while (it != mItems.end())
		{
			QuadTreeNodeItem *item = *it;

			bool itemInserted = false;

			for (auto &subNode: mSubNodes)
			{
				itemInserted = subNode->insertItem(item);
				if (itemInserted)
				{
					// inserted item into sub node!
					// remove from this (parent) node.
					it = mItems.erase(it);
					break;
				}
			}

			if (!itemInserted)
			{
				it++;
			}
		}

		mHaveSubNodes = true;

		log_debug("split node: %s end: numItems=%s", mBounds, mItems.size());

		if (mEntity != nullptr)
		{
			//mEntity->setParent((Qt3DCore::QNode*)nullptr);
			delete mEntity;
			mEntity = nullptr;
		}

		for (auto &subNode: mSubNodes)
		{
			subNode->updateGeometry();
		}
	}
}

QuadTreeNodeGeometry *QuadTreeNode::getGeometry()
{
	return mGeometry;
}


