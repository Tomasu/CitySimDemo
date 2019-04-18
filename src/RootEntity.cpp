//
// Created by moose on 17/04/19.
//

#include "RootEntity.h"

#include "LogUtils.h"
#define TAG "RootEntity"

void RootEntity::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change)
{
	log_debug("scene change event: %s", change);

	QEntity::sceneChangeEvent(change);
}

RootEntity::RootEntity(Qt3DCore::QNode *parent) : QEntity(parent)
{

}

RootEntity::~RootEntity()
{

}
