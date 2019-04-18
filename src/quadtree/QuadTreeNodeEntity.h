//
// Created by moose on 05/04/19.
//

#ifndef CITYSIM_QUADTREENODEENTITY_H
#define CITYSIM_QUADTREENODEENTITY_H

#include <Qt3DCore/QNode>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QMaterial>
#include <Qt3DCore/QTransform>
#include "QuadTreeNodeGeometryRenderer.h"

class QuadTreeNodeEntity;

class QuadTreeNodeEntity : public Qt3DCore::QEntity
{
	public:
		explicit QuadTreeNodeEntity(QuadTreeNode *node, Qt3DCore::QNode *parent = nullptr);
		~QuadTreeNodeEntity() override;

		QuadTreeNodeGeometryRenderer *renderer() { return mRenderer; }

	protected:
		void
		sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override;

	private:
		Qt3DCore::QTransform *mTransform;
		Qt3DRender::QMaterial *mMaterial;
		QuadTreeNodeGeometryRenderer *mRenderer;
};


#endif //CITYSIM_QUADTREENODEENTITY_H
