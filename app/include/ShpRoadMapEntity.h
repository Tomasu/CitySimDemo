//
// Created by moose on 03/04/19.
//

#ifndef CITYSIM_SHPROADMAPENTITY_H
#define CITYSIM_SHPROADMAPENTITY_H

#include <ogrsf_frmts.h>
#include "ShpEntity.h"

class SHPRoadMapEntity : public SHPEntity
{
	public:
		explicit SHPRoadMapEntity(QNode *parent = nullptr);
		virtual ~SHPRoadMapEntity() override;

		bool load(GDALDataset *dataset) override;


};


#endif //CITYSIM_SHPROADMAPENTITY_H
