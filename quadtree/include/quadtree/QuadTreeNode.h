//
// Created by moose on 04/04/19.
//

#ifndef CITYSIM_QUADTREENODE_H
#define CITYSIM_QUADTREENODE_H

//#include "core/Plane3D.h"
#include "util/Rect.h"
#include "util/Point.h"
#include <cstdint>
#include <vector>
#include <memory>
#include <QDebug>
#include <mutex>
#include <Qt3DRender/QGeometry>

class QuadTreeNodeEntity;
class QuadTreeNodeItem;
class QuadTreeNodeGeometry;
class QuadTreeNodeGeometryRenderer;
class QuadTreeNodeEntityFactory;

class QuadTreeNodeMapper
{
	public:
		QuadTreeNodeMapper() {}
		virtual ~QuadTreeNodeMapper() {}

		virtual Point map(const Point &vec) = 0;
};

class QuadTreeNode
{
	public:
		// when to split and merge nodes
		const static int32_t NODE_SPLIT_COUNT = 10240;
		const static uint32_t NODE_MERGE_COUNT = 8;

		QuadTreeNode(const QuadTreeNode&) = delete;

		explicit QuadTreeNode(QuadTreeNode *parent, QuadTreeNodeEntityFactory *entityFactory, const Rect &bounds, QuadTreeNodeMapper *mapper, size_t nodeSplitCount = NODE_SPLIT_COUNT);

		virtual ~QuadTreeNode();

		QuadTreeNode *getParent() { return mParent; }
		void setParent(QuadTreeNode *parent) { mParent = parent; }

		bool haveItems() const { return mItems.size() > 0; }
		uint32_t getItemCount() const { return mItems.size(); }

		bool insertItem(QuadTreeNodeItem *item, bool noupdate = false);

		void removeItem(QuadTreeNodeItem */* item */);

		const std::vector<QuadTreeNodeItem*> &getItems() { return mItems; }

		const QuadTreeNodeItem *itemAt(int idx) const
		{
			return mItems.at(idx);
		}

		template<typename T>
		T *itemAs(int idx)
		{
			return dynamic_cast<T*>(mItems.at(idx));
		}

		QuadTreeNodeGeometry *getGeometry();

		QuadTreeNodeGeometry *createGeometry();

		const Rect &bounds() const { return mBounds; }

		bool hasSubNodes() const { return mHaveSubNodes; }

		bool hasParent() const { return mParent != nullptr; }

		Point map(const Point &vec);

		QuadTreeNode *topLeft() const { return mSubNodes[0]; }
		QuadTreeNode *topRight() const { return mSubNodes[1]; }
		QuadTreeNode *bottomLeft() const { return mSubNodes[2]; }
		QuadTreeNode *bottomRight() const { return mSubNodes[3]; }

		const std::vector<QuadTreeNode*> &subNodes() const { return mSubNodes; }

		void dumpNode(const std::string &prefix);

		void updateGeometry();

		bool contains(const Rect &rect);
		bool intersects(const Rect &rect);

	protected:
		void splitNode();


	private:
		QuadTreeNode *mParent;

		Rect mBounds;
		bool mHaveSubNodes;
		QuadTreeNodeEntityFactory *mEntityFactory;
		const size_t mNodeSplitCount;

		// ptr to mItems vector: ptr vs no ptr, that is the question.
		//  ptr == higher access overhead
		//  no-ptr == higher memory overhead per node
		std::vector<QuadTreeNodeItem*> mItems;

		std::vector<QuadTreeNode*> mSubNodes;

		QuadTreeNodeGeometry *mGeometry;
		QuadTreeNodeEntity *mEntity;

		std::mutex mMutex;

		QuadTreeNodeMapper *mMapper;

		//Plane3D mPlane;
};

#endif //CITYSIM_QUADTREENODE_H
