//
// Created by moose on 18/04/19.
//

#include "geometryloader/GdalLoaderFactory.h"
#include "geometryloader/GdalLoader.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <gdal_priv.h>

#include "util/LogUtils.h"

#define TAG "GDALLoaderFactory"

QString shpPath(const QFileInfo &fi);


GeometryLoader *GDALLoaderFactory::create(const std::string &path) const
{
	if (!path.empty())
	{
		QFileInfo fileInfo(path.c_str());
		if (fileInfo.isDir())
		{
			log_debug("got dir: %s", path);

			return new GDALLoader(path);
		}
	}

	return nullptr;
}
