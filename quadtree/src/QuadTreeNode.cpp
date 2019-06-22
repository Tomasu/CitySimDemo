#include <memory>
#include "LogUtils.h"

//
// Created by moose on 04/04/19.
//

#include "quadtree/QuadTreeNode.h"
#include "quadtree/QuadTreeNodeItem.h"
#include "quadtree/QuadTreeNodeEntity.h"
#include "quadtree/QuadTreeNodeEntityFactory.h"
#include "quadtree/QuadTreeNodeGeometry.h"
#include "quadtree/QuadTreeNodeGeometryRenderer.h"

#include "util/Size.h"

#include <QVector3D>

#define TAG "QuadTreeNode"

QuadTreeNode::QuadTreeNode(QuadTreeNode *parent, QuadTreeNodeEntityFactory *entityFactory, const Rect &bounds, QuadTreeNodeMapper *mapper, size_t nodeSplitCount)
		: mParent{parent},
		mBounds{bounds},
		mHaveSubNodes{false},
		mEntityFactory(entityFactory),
		mNodeSplitCount{nodeSplitCount},
		mEntity{nullptr},
		mMapper{mapper},
		mPlane{bounds, 0.0f}
{
	mGeometry = nullptr;
}

QuadTreeNode::~QuadTreeNode()
{
	//std::scoped_lock<std::mutex> lock(mMutex);

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

	mParent = nullptr;


	// if we change mItems to a ptr, delete it here
}

bool QuadTreeNode::insertItem(QuadTreeNodeItem *item, bool noupdate)
{
	// may want to make this lock a little more granular later?
	//std::scoped_lock<std::mutex> lock(mMutex);

	const Rect &bounds = item->bounds();
// if item's center is inside this rect
	if (item->isContainedByRect(mBounds))
	{
		if (!mHaveSubNodes && mItems.size() >= mNodeSplitCount)
		{
			mItems.push_back(item);

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
			//item->setOffset(-mBounds.topLeft());
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

	if (mItems.size() > 0)
	{
		if (mEntity != nullptr)
		{
			log_debug("update geom");
			//log_debug("update geom here!");
			mEntity->renderer()->setGeometryFactory(mEntity->renderer()->geometryFactory());
		}
		else
		{
			log_debug("create entity/geom: items=%s", mItems.size());
			mEntity = (*mEntityFactory)(this);
			mEntity->renderer()->setGeometryFactory(mEntity->renderer()->geometryFactory());
		}
	}

	for (auto *subNode: mSubNodes)
	{
		subNode->updateGeometry();
	}
}

void QuadTreeNode::removeItem(QuadTreeNodeItem *)
{
	// TODO: implement me!
	throw std::runtime_error("unimplemented!");
}

QuadTreeNodeGeometry* QuadTreeNode::createGeometry()
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

static bool itemIntersects(QuadTreeNode *node, QuadTreeNodeItem *item)
{
	Rect nodeBounds = node->bounds();
	return //!node->contains(item->bounds()) &&
		item->isPartiallyContainedByRect(nodeBounds);
}

#include "RoadData.h"

void QuadTreeNode::splitNode()
{
	//log_debug("haveSubNodes: %s", mHaveSubNodes ? "true" : "false");

	// initial implementation will just hold the lock during the entire process
	//  it is possible we could take a copy of the data and mark this node as read-only
	//  till the process is done and swap in the new data after. and only hold locks
	//  during the initial copy and final copy.
	if (!mHaveSubNodes)
	{
		//std::scoped_lock<std::mutex> lock(mMutex);

		mHaveSubNodes = true;

		log_debug("split node: %s begin: numItems=%s", mBounds, mItems.size());

		Size size = mBounds.size() / 2;

		Rect topLeft{ mBounds.topLeft(), size };
		Rect topRight{ {mBounds.center().x(), mBounds.top()}, size };
		Rect bottomLeft{ {mBounds.left(), mBounds.center().y()}, size };
		Rect bottomRight{ mBounds.center(), size };

		log_debug("new quads: %s %s %s %s", topLeft, topRight, bottomLeft, bottomRight);

		QuadTreeNode *topLeftNode = new QuadTreeNode(this, mEntityFactory, topLeft, mMapper);
		mSubNodes.push_back(topLeftNode);

		QuadTreeNode *topRightNode = new QuadTreeNode(this, mEntityFactory, topRight, mMapper);
		mSubNodes.push_back(topRightNode);

		QuadTreeNode *bottomLeftNode = new QuadTreeNode(this, mEntityFactory, bottomLeft, mMapper);
		mSubNodes.push_back(bottomLeftNode);

		QuadTreeNode *bottomRightNode = new QuadTreeNode(this, mEntityFactory, bottomRight, mMapper);
		mSubNodes.push_back(bottomRightNode);

		// copy list to avoid seeing changes to mItems while we're processing
		//  and so we can append items as we process.
		std::vector<QuadTreeNodeItem*> queue(mItems);

		Rect nodeBounds = bounds();
		Point nodeCenter = nodeBounds.center();

		QVector<QVector3D> newPoints;

		auto it = queue.begin();
		for (size_t i = 0; i < queue.size(); i++)
		{
			QuadTreeNodeItem *item = queue.at(i);

			if (!item->isContainedByRect(nodeBounds))
			{
				log_error("item is not contained by bounds?! %s < %s", item->bounds(), nodeBounds);
				throw std::runtime_error("item is not contained by bounds?!?!");
			}

			bool itemInserted = false;

			bool isContainedByTopLeft = item->isContainedByRect(topLeft);
			bool isContainedByTopRight = item->isContainedByRect(topRight);
			bool isContainedByBottomLeft = item->isContainedByRect(bottomLeft);
			bool isContainedByBottomRight = item->isContainedByRect(bottomRight);
			bool itemFullyContained = isContainedByTopLeft || isContainedByTopRight || isContainedByBottomLeft || isContainedByBottomRight;

			std::string containedStr;

			if (isContainedByTopLeft)
			{
				containedStr += "top left";
			}

			if (isContainedByTopRight)
			{
				if (containedStr.size() > 0)
				{
					containedStr += ", ";
				}
				containedStr += "top right";
			}

			if (isContainedByBottomLeft)
			{
				if (containedStr.size() > 0)
				{
					containedStr += ", ";
				}
				containedStr += "bottom left";
			}

			if (isContainedByBottomRight)
			{
				if (containedStr.size() > 0)
				{
					containedStr += ", ";
				}
				containedStr += "bottom right";
			}

			if (!containedStr.empty())
			{
				log_debug("is contained by: %s", containedStr);
			}
			else
			{
				log_error("somehow this item isnt contained by any sub node?!?!?!");
				log_debug("item: %s", item);
				log_debug("nodes: %s %s %s %s", topLeft, topRight, bottomLeft, bottomRight);
				//throw std::runtime_error("WAT");
			}

			if (itemFullyContained)
			{
				log_debug("item is fully contained...");
			}
			else if (itemIntersects(bottomRightNode, item)
				|| itemIntersects(bottomLeftNode, item)
				|| itemIntersects(topRightNode, item)
				|| itemIntersects(topLeftNode, item))
			{
				// if this item intersects and is not entirely contained in any sub node
				//  split item into separate pieces, append to queue and skip to next item.

				log_debug("new intersection...");
				//log_debug("item intersected with a new subnode during node split. splitting and appending to queue...");

				log_debug("old item: %s", item->toString());

				std::vector<QuadTreeNodeItem*> splits = item->split(nodeBounds, nodeCenter);

				log_debug("got %s splits: %s", splits.size(), splits);

				if (splits.size() < 2)
				{
					log_debug("got less than 2 splits, this is an error >:(");
					throw std::runtime_error("WAH!");
				}

				// remove this item from queue and advance to next item.
				//it = queue.erase(it);

				// append new items to queue.
				queue.insert(queue.end(), splits.begin(), splits.end());
// 				int ii = i;
// 				for (auto split: splits)
// 				{
// 					queue.insert(ii+1, split);
// 					ii++;
// 				}

				continue;
			}

			for (auto &subNode: mSubNodes)
			{
				if (item->isContainedByRect(subNode->bounds()))
				{
					itemInserted = subNode->insertItem(item, true);
					if (itemInserted)
					{
						// inserted item into sub node!
						// remove from this (parent) node.
						//it = queue.erase(it);
						break;
					}
				}
			}

			if (!itemInserted)
			{
				//it++;
				log_debug("failed to insert line???");
			}
		}

		mHaveSubNodes = true;

		// this is technically wrong. sigh.
		//  if mItems gets modified outside of this method in another thread
		//  we'll loose that modification.
		// FIXME: we definitely need to keep from running splitNode more than once at a time.
		mItems = queue;

		log_debug("split node: %s end: numItems=%s", mBounds, mItems.size());

		if (mEntity != nullptr)
		{
			mEntity->setParent((Qt3DCore::QNode*)nullptr);
			delete mEntity;
			mEntity = nullptr;
		}

		if (mGeometry != nullptr)
		{
			delete mGeometry;
			mGeometry = nullptr;
		}

// 		for (auto &subNode: mSubNodes)
// 		{
// 			subNode->updateGeometry();
// 		}
	}

	// TODO: I think this will leave spurious data in parent nodes...
	// need to remove it after?
}

QuadTreeNodeGeometry *QuadTreeNode::getGeometry()
{
	return mGeometry;
}

QVector3D QuadTreeNode::map(const QVector3D& vec)
{
	return mMapper->map(vec);
}

bool QuadTreeNode::contains(const Rect& rect)
{
	return bounds().contains(rect);
}

bool QuadTreeNode::intersects(const Rect& rect)
{
	return bounds().intersects(rect);
}


void QuadTreeNode::dumpNode(const std::string &prefix)
{
	log_debug("%s%s", prefix.c_str(), bounds());

	for (auto *node: subNodes())
	{
		node->dumpNode(prefix + "   ");
	}
}


