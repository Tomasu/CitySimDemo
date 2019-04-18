//
// Created by moose on 03/04/19.
//

#include <ogrsf_frmts.h>

#include "ShpRoadMapEntity.h"

using namespace Qt3DCore;

SHPRoadMapEntity::SHPRoadMapEntity(QNode *parent) : SHPEntity(parent)
{

}

SHPRoadMapEntity::~SHPRoadMapEntity()
{

}

bool SHPRoadMapEntity::load(GDALDataset *dataset)
{
	return SHPEntity::load(dataset);
}
