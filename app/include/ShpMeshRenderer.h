//
// Created by moose on 30/03/19.
//

#ifndef CITYSIM_SHPMESHRENDERER_H
#define CITYSIM_SHPMESHRENDERER_H

#include <ogrsf_frmts.h>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QMaterial>
#include <Qt3DCore/QTransform>
#include "ShpMeshGeometry.h"

class SHPMeshRenderer : public Qt3DRender::QGeometryRenderer
{
	public:
		SHPMeshRenderer(Qt3DCore::QNode *parent = nullptr);
		~SHPMeshRenderer();

		bool load(GDALDataset *dataset);

		SHPMeshGeometry *shpGeometry() { return mGeometry; }

	private:
		SHPMeshGeometry *mGeometry;
};


#endif //CITYSIM_SHPMESHRENDERER_H
