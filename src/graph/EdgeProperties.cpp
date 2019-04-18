//
// Created by moose on 03/04/19.
//

#include "EdgeProperties.h"

uint32_t EdgeProperties::getId() const
{
	return mId;
}

void EdgeProperties::setId(uint32_t id)
{
	mId = id;
}

int EdgeProperties::getWeight() const
{
	return mWeight;
}

void EdgeProperties::setWeight(int weight)
{
	mWeight = weight;
}
