//
// Created by moose on 30/03/19.
//

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <Qt3DExtras/QPerVertexColorMaterial>

#include "ShpEntity.h"
#include "ShpMeshRenderer.h"

using namespace Qt3DCore;
using namespace Qt3DRender;

SHPEntity::SHPEntity(Qt3DCore::QNode *parent) : QEntity(parent)
{

}

SHPEntity::~SHPEntity()
{

}

bool SHPEntity::load(const QString &path)
{
	QFileInfo fi(path);
	setObjectName(fi.dir().dirName());

	mTransform = new Qt3DCore::QTransform;
	//mTransform->setScale(0.01f);
	mTransform->setTranslation(QVector3D(0.0f, 0.0f, 0.0f));

	mMaterial = new Qt3DExtras::QPerVertexColorMaterial(this);

	mShpRenderer = new SHPMeshRenderer();
	if(!mShpRenderer->load(path))
	{
		return false;
	}

	addComponent(mShpRenderer);
	addComponent(mTransform);
	addComponent(mMaterial);

	setEnabled(true);

	return true;
}
