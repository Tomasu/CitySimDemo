//
// Created by moose on 18/04/19.
//

#include "geometryloader/GdalLoader.h"

#include <QtCore/QString>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <gdal_priv.h>


#include "util/LogUtils.h"
#define TAG "GDALLoader"


GDALLoader::GDALLoader(const std::string &path) : mPath(path)
{

}

std::string shpPath(const QFileInfo &fi);
GDALDataset *openDataset(const std::string &path);

bool GDALLoader::load()
{
	if (!mPath.empty())
	{
		QFileInfo fileInfo(mPath.c_str());
		if (fileInfo.isDir())
		{
			log_debug("got dir: %s", mPath);

			std::string shpPathStr = shpPath(fileInfo);

			return doLoad(shpPathStr);
		}
	}

	return false;
}

bool GDALLoader::doLoad(const std::string &shpPath)
{
	GDALDataset *dataset = openDataset(shpPath);
	if (dataset == nullptr)
	{
		return false;
	}




	return true;
}

GDALDataset *openDataset(const std::string &path)
{
	std::string realPath = path + ".shp";
	auto *dataset = static_cast<GDALDataset *>(GDALOpenEx(realPath.c_str(),
			GDAL_OF_READONLY | GDAL_OF_VECTOR | GDAL_OF_VERBOSE_ERROR, nullptr, nullptr, nullptr));

	if (dataset == nullptr)
	{
		log_error("error opening with gdal?!");
		return nullptr;
	}

	return dataset;
}

std::string shpPath(const QFileInfo &fi)
{
	if(!fi.isDir())
	{
		log_error("not a dir!");
		return "";
	}

	QDir dir(fi.filePath());

	QFileInfoList fileInfoList = dir.entryInfoList(
			QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);

	for (QFileInfo entInfo: fileInfoList)
	{
		if (entInfo.suffix() == "shp" || entInfo.suffix() == "shx")
		{
			QFileInfo fileInfo(entInfo.dir(), entInfo.baseName());
			log_debug("found: %s", qPrintable(fileInfo.filePath()));
			return fileInfo.filePath().toStdString();
		}
	}

	return "";
}
