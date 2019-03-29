//
// Created by moose on 28/03/19.
//

#ifndef CITYSIM_SHPMESHGEOMETRY_H
#define CITYSIM_SHPMESHGEOMETRY_H

#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>

class SHPMeshGeometry : public Qt3DRender::QGeometry
{
	public:
		const static int32_t VTX_POS_SIZE = 3;
		const static int32_t VTX_COLOR_SIZE = 3;
		const static int32_t ELEMENT_SIZE = VTX_POS_SIZE + VTX_COLOR_SIZE;
		const static int32_t ELEMENT_STRIDE = ELEMENT_SIZE * sizeof(float);
		const static int32_t IDX_SIZE = 1;


		SHPMeshGeometry(const QString &path, Qt3DCore::QNode *parent);
		~SHPMeshGeometry();

		int getNumVerticies() { return mNumVerticies; }

	private:
		Qt3DRender::QAttribute *mPosAttribute;
		Qt3DRender::QAttribute *mColorAttribute;
		Qt3DRender::QAttribute *mIndexAttribute;
		Qt3DRender::QBuffer *mVertexBuffer;
		Qt3DRender::QBuffer *mIndexBuffer;
		int mNumVerticies;

		void loadMesh(const QString &qString);
};


#endif //CITYSIM_SHPMESHGEOMETRY_H
