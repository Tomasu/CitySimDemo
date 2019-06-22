//
// Created by moose on 05/04/19.
//

#include <Qt3DExtras/QPerVertexColorMaterial>
#include "QuadTreeNode.h"
#include "QuadTreeNodeEntity.h"
#include "QuadTreeNodeGeometryFactory.h"
#include "QuadTreeNodeGeometryRenderer.h"
#include "util/Rect.h"

#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickLineEvent>

#include <Qt3DInput/QMouseHandler>
#include <Qt3DInput/QMouseDevice>

#include <Qt3DCore/QTransform>

#include <Qt3DExtras/QPerVertexColorMaterial>

#include "RoadData.h"
#include "MainWindow.h"

#include "LogUtils.h"
#define TAG "QuadTreeNodeEntity"

QuadTreeNodeEntity::QuadTreeNodeEntity(QuadTreeNode *node, MainWindow *mw, Qt3DCore::QNode *parent)
	: QEntity(parent), mNode{node}, mPicker{nullptr}
{
	log_trace_enter();

// 	float scale = 1.0f;
// 	mScale = new Qt3DCore::QTransform;
// 	mScale->setScale(scale);

	mTranslation = new Qt3DCore::QTransform(this);
	log_debug("entity bounds: %s", node->bounds());
	QMatrix4x4 matrix; matrix.translate(node->bounds().left(), node->bounds().top());
	matrix.scale(1.0f);
	mTranslation->setMatrix(matrix);

	mMaterial = new Qt3DExtras::QPerVertexColorMaterial(this);

	mRenderer = new QuadTreeNodeGeometryRenderer(new QuadTreeNodeGeometryFactory(node), this);

	mMainWindow = mw;

	addComponent(mTranslation);
	//addComponent(mScale);
	addComponent(mRenderer);
	addComponent(mMaterial);

	//addComponent(mMainWindow->mouseHandler());

	//QObject::connect(mMouseHandler, &Qt3DInput::QMouseHandler::positionChanged, this, &QuadTreeNodeEntity::mousePositionChanged);


// 	mPicker = new Qt3DRender::QObjectPicker(this);
// 	mPicker->setHoverEnabled(true);
// 	mPicker->setDragEnabled(true);
// 	addComponent(mPicker);
//
// 	connect(mPicker, &Qt3DRender::QObjectPicker::moved, this, &QuadTreeNodeEntity::objectPickerMoved);

	setEnabled(true);

	log_trace_exit();
}

void QuadTreeNodeEntity::objectPickerMoved(Qt3DRender::QPickEvent *ev)
{
	log_debug("node: %s", mNode->bounds());
	log_debug("ev: pos: %s dist: %s", ev->position(), ev->distance());
	log_debug("ev: local: %s", ev->localIntersection());
	log_debug("ev: world: %s", ev->worldIntersection());
	// 		auto items = mNode->getItems();
	// 		for (auto &item: items)
	// 		{
	// 			RoadData *roadData = (RoadData*)item;
	// 			for (const TransportGraph::Edge &edge: roadData->getFwdEdges())
	// 			{
	// 				if (edge.m_target)
	// 				{
	// 					edge.m_target.
	// 				}
	// 			}
	// 		}
	ev->setAccepted(true);
}


QuadTreeNodeEntity::~QuadTreeNodeEntity()
{
	//log_trace_exit();
}

void QuadTreeNodeEntity::mousePositionChanged(Qt3DInput::QMouseEvent* ev)
{
	//if (mMouseHandler->containsMouse())
	{
		QVector3D mapped = mNode->map({(float)ev->x(), (float)ev->y(), 0.0f});

		Rect bounds = mNode->bounds();
		//log_debug("bounds: %sx%s %sx%s ms: %sx%s", bounds.left(), bounds.top(), bounds.right(), bounds.bottom(), mapped.x(), mapped.y());
		if (mapped.x() >= bounds.left()
			&& mapped.x() <= bounds.right()
			&& mapped.y() >= bounds.top()
			&& mapped.y() <= bounds.bottom()
		)
		{
			log_debug("%s ev: %s mapped: %s", bounds, ev, mapped);
		}
	}
}


void QuadTreeNodeEntity::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change)
{
	log_trace_enter();
	QNode::sceneChangeEvent(change);
	log_debug("sceneChangeEvent: %s", change);
	log_trace_exit();
}
