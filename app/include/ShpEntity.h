//
// Created by moose on 30/03/19.
//

#ifndef CITYSIM_SHPENTITY_H
#define CITYSIM_SHPENTITY_H

#include <gdal_frmts.h>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QObjectPicker>
#include "ShpMeshRenderer.h"

class SHPEntity : public Qt3DCore::QEntity
{
	public:
		explicit SHPEntity(QNode *parent = nullptr);
		virtual ~SHPEntity() override;

		virtual bool load(GDALDataset *dataset);
		Qt3DCore::QTransform *getTransform() { return mTransform; }
		Qt3DRender::QMaterial *getMaterial() { return mMaterial; }

		double getOriginX() { return mShpRenderer->shpGeometry()->originX(); }
		double getOriginY() { return mShpRenderer->shpGeometry()->originY(); }

		QVector3D origin() { return QVector3D(getOriginX(), getOriginY(), 0.0f); }

	private:
		Qt3DCore::QTransform *mTransform;
		Qt3DRender::QMaterial *mMaterial;
		SHPMeshRenderer *mShpRenderer;
		Qt3DRender::QObjectPicker *mPicker;
};


#endif //CITYSIM_SHPENTITY_H
