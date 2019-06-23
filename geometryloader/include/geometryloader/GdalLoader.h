//
// Created by moose on 18/04/19.
//

#ifndef CITYSIM_GDALLOADER_H
#define CITYSIM_GDALLOADER_H


#include <string>
#include "geometryloader/GeometryLoader.h"

class GDALLoader : public GeometryLoader
{
	public:
		explicit GDALLoader(const std::string &path);

		bool load() override;

	private:
		std::string mPath;

		bool doLoad(const std::string &shpPath);
};


#endif //CITYSIM_GDALLOADER_H
