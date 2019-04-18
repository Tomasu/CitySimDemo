//
// Created by moose on 04/04/19.
//

#ifndef CITYSIM_QUADTREENODE_H
#define CITYSIM_QUADTREENODE_H

#include <cstdint>
#include <vector>
#include <memory>
#include <QtCore/QRectF>
#include <QDebug>
#include <mutex>
#include <Qt3DRender/QGeometry>

class QuadTreeNodeEntity;
class QuadTreeNodeItem;
class QuadTreeNodeGeometry;
class QuadTreeNodeGeometryRenderer;
class QuadTreeNodeEntityFactory;

class QuadTreeNode
{
	public:
		// when to split and merge nodes
		const static int32_t NODE_SPLIT_COUNT = 1024;
		const static uint32_t NODE_MERGE_COUNT = 8;

		QuadTreeNode(const QuadTreeNode&) = delete;

		explicit QuadTreeNode(QuadTreeNodeEntityFactory *entityFactory, const QRectF &bounds);

		virtual ~QuadTreeNode();

		bool haveItems() const { return mItems.size() > 0; }
		uint32_t getItemCount() const { return mItems.size(); }

		bool insertItem(QuadTreeNodeItem *item, bool noupdate = false);

		void removeItem(QuadTreeNodeItem */* item */);

		const QVector<QuadTreeNodeItem*> &getItems() { return mItems; }

		QuadTreeNodeGeometry *getGeometry();

		Qt3DRender::QGeometry *createGeometry();

		const QRectF &bounds() const { return mBounds; }

		bool hasSubNodes() const { return mHaveSubNodes; }

	protected:
		void splitNode();

		void updateGeometry();

	private:
		QRectF mBounds;
		bool mHaveSubNodes;
		QuadTreeNodeEntityFactory *mEntityFactory;

		// ptr to mItems vector: ptr vs no ptr, that is the question.
		//  ptr == higher access overhead
		//  no-ptr == higher memory overhead per node
		QVector<QuadTreeNodeItem*> mItems;

		QVector<QuadTreeNode*> mSubNodes;

		QuadTreeNodeGeometry *mGeometry;
		QuadTreeNodeEntity *mEntity;


		std::mutex mMutex;
};

#endif //CITYSIM_QUADTREENODE_H
