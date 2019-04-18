//
// Created by moose on 28/03/19.
//

#ifndef CITYSIM_SHPMESHGEOMETRY_H
#define CITYSIM_SHPMESHGEOMETRY_H

#include <gdal_priv.h>
#include <gdal.h>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>

class SHPMeshGeometry : public Qt3DRender::QGeometry
{
	public:
		SHPMeshGeometry(GDALDataset *dataset, Qt3DCore::QNode *parent);
		~SHPMeshGeometry();

		int getNumVerticies() { return mNumVerticies; }

		double originX() { return mOriginX; }
		double originY() { return mOriginY; }

	private:
		Qt3DRender::QAttribute *mPosAttribute;
		Qt3DRender::QAttribute *mColorAttribute;
		//Qt3DRender::QAttribute *mIndexAttribute;
		Qt3DRender::QBuffer *mVertexBuffer;
		Qt3DRender::QBuffer *mIndexBuffer;
		int mNumVerticies;

		double mOriginX;
		double mOriginY;

		void loadMesh(GDALDataset *dataset);
};


#endif //CITYSIM_SHPMESHGEOMETRY_H
