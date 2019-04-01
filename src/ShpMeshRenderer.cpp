//
// Created by moose on 30/03/19.
//

#include "ShpMeshRenderer.h"
#include "Constants.h"

#include <Qt3DCore/QTransform>
#include <Qt3DRender/QMaterial>
using namespace Qt3DCore;
using namespace Qt3DRender;

SHPMeshRenderer::SHPMeshRenderer(Qt3DCore::QNode *parent) : QGeometryRenderer(parent)
{
	// maybe should be parent? original loadShp hands this the entity not the renderer
	mGeometry = nullptr;
}

SHPMeshRenderer::~SHPMeshRenderer()
{

}

bool SHPMeshRenderer::load(const QString &path)
{
	mGeometry = new SHPMeshGeometry(path, this);

	//setInstanceCount(20);
	//setIndexOffset(0);
	//setFirstInstance(0);
	setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
	setPrimitiveRestartEnabled(true);
	setRestartIndexValue(VTX_IDX_RESTART_ID);

	setGeometry(mGeometry);
	setVertexCount(mGeometry->getNumVerticies());

	return true;
}

