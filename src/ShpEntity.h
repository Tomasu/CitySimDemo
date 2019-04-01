//
// Created by moose on 30/03/19.
//

#ifndef CITYSIM_SHPENTITY_H
#define CITYSIM_SHPENTITY_H


#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QGeometryRenderer>
#include "ShpMeshRenderer.h"

class SHPEntity : public Qt3DCore::QEntity
{
	public:
		explicit SHPEntity(QNode *parent = nullptr);
		~SHPEntity() override;

		bool load(const QString &path);
		Qt3DCore::QTransform *getTransform() { return mTransform; }
		Qt3DRender::QMaterial *getMaterial() { return mMaterial; }

		double getOriginX() { return mShpRenderer->shpGeometry()->originX(); }
		double getOriginY() { return mShpRenderer->shpGeometry()->originY(); }

		QVector3D origin() { return QVector3D(getOriginX(), getOriginY(), 0.0f); }

	private:
		Qt3DCore::QTransform *mTransform;
		Qt3DRender::QMaterial *mMaterial;
		SHPMeshRenderer *mShpRenderer;
};


#endif //CITYSIM_SHPENTITY_H
