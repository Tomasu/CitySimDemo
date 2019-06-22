//
// Created by moose on 18/04/19.
//

#ifndef CITYSIM_GEOMETRYLOADERFACTORY_H
#define CITYSIM_GEOMETRYLOADERFACTORY_H

#include <string>
#include "GeometryLoader.h"

class GeometryLoaderFactory
{
	private:
		static constexpr int NUM_FACTORIES = 1;
		static const GeometryLoaderFactory *FACTORIES[NUM_FACTORIES];

	public:
		static GeometryLoader *Create(const std::string &path);

		virtual GeometryLoader *create(const std::string &path) const = 0;
};


#endif //CITYSIM_GEOMETRYLOADERFACTORY_H
