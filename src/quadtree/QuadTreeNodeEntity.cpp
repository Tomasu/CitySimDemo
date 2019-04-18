//
// Created by moose on 05/04/19.
//

#include <Qt3DExtras/QPerVertexColorMaterial>
#include <src/LogUtils.h>
#include "quadtree/QuadTreeNodeEntity.h"
#include "quadtree/QuadTreeNodeGeometryFactory.h"

#define TAG "QuadTreeNodeEntity"

QuadTreeNodeEntity::QuadTreeNodeEntity(QuadTreeNode *node, Qt3DCore::QNode *parent) : QEntity(parent)
{
	log_trace_enter();

	mTransform = new Qt3DCore::QTransform;
	mTransform->setTranslation(QVector3D(node->bounds().x(), node->bounds().y(), 0.0f));

	mMaterial = new Qt3DExtras::QPerVertexColorMaterial(this);

	mRenderer = new QuadTreeNodeGeometryRenderer(new QuadTreeNodeGeometryFactory(node), this);

	log_trace_exit();
}

QuadTreeNodeEntity::~QuadTreeNodeEntity()
{
	//log_trace_exit();
}

void QuadTreeNodeEntity::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change)
{
	log_debug("sceneChangeEvent: %s", change);
	QNode::sceneChangeEvent(change);
}
