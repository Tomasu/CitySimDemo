//
// Created by moose on 18/04/19.
//

#ifndef CITYSIM_GDALLOADERFACTORY_H
#define CITYSIM_GDALLOADERFACTORY_H


#include "GeometryLoaderFactory.h"

class GDALLoaderFactory : public GeometryLoaderFactory
{
	public:
		GeometryLoader *create(const std::string &path) const override;
};


#endif //CITYSIM_GDALLOADERFACTORY_H
