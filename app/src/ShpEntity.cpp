//
// Created by moose on 30/03/19.
//

#include <ogrsf_frmts.h>

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <Qt3DExtras/QPerVertexColorMaterial>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickLineEvent>

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

bool SHPEntity::load(GDALDataset *dataset)
{
	mTransform = new Qt3DCore::QTransform;
	//mTransform->setScale(0.01f);
	mTransform->setTranslation(QVector3D(0.0f, 0.0f, 0.0f));

	mMaterial = new Qt3DExtras::QPerVertexColorMaterial(this);

	mShpRenderer = new SHPMeshRenderer();
	if(!mShpRenderer->load(dataset))
	{
		return false;
	}

	mPicker = new Qt3DRender::QObjectPicker;

	addComponent(mPicker);
	addComponent(mShpRenderer);
	addComponent(mTransform);
	addComponent(mMaterial);


	connect(mPicker, &Qt3DRender::QObjectPicker::clicked, this, [&](Qt3DRender::QPickEvent *e) {
		auto p = dynamic_cast<Qt3DRender::QPickLineEvent*>(e);
		auto idx = p->edgeIndex();
		qDebug() << " pick: idx=" << idx << ": " << p;
		e->setAccepted(true);
	});

	setEnabled(true);

	return true;
}
