//
// Created by moose on 18/04/19.
//

#include <stdexcept>
#include "geometryloader/GeometryLoaderFactory.h"

#include "geometryloader/GdalLoaderFactory.h"

const GeometryLoaderFactory *GeometryLoaderFactory::FACTORIES[NUM_FACTORIES] = {
		new GDALLoaderFactory()
};

GeometryLoader *GeometryLoaderFactory::Create(const std::string &path)
{
	for (const GeometryLoaderFactory *factory: FACTORIES)
	{
		const GeometryLoader *loader = factory->create(path);
		if (loader != nullptr)
		{
			return const_cast<GeometryLoader*>(loader);
		}
	}

	return nullptr;
}
