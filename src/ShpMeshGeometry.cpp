//
// Created by moose on 28/03/19.
//

#include <Qt3DRender/QBuffer>
#include <QVector4D>
#include <shapefil.h>
#include <QtGui/QVector3D>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <QtCore/QSizeF>
#include <gdal_priv.h>
#include <ogrsf_frmts.h>
#include <QtGui/QPainter>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <set>
#include "ShpMeshGeometry.h"
#include "LocUtil.h"
#include "GeometryBuilder.h"
#include "Constants.h"

using namespace Qt3DRender;
using namespace Qt3DCore;

SHPMeshGeometry::SHPMeshGeometry(GDALDataset *dataset, QNode *parent) : QGeometry(parent)
{
	mNumVerticies = 0;
	mVertexBuffer = new QBuffer(Qt3DRender::QBuffer::VertexBuffer, this);
	mVertexBuffer->setAccessType(QBuffer::Read);
	mVertexBuffer->setUsage(QBuffer::StaticDraw);

	mIndexBuffer = new QBuffer(Qt3DRender::QBuffer::IndexBuffer, this);

	mPosAttribute = new Qt3DRender::QAttribute(this);
	mPosAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
	//posAttribute->setBuffer(vertexBuffer);
	mPosAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
	mPosAttribute->setVertexSize(VTX_POS_SIZE);
	mPosAttribute->setByteOffset(0);
	mPosAttribute->setByteStride(VTX_STRIDE);
	//posAttribute->setCount(numVerticies);
	mPosAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

	mColorAttribute = new Qt3DRender::QAttribute(this);
	mColorAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
	//mColorAttribute->setBuffer(vertexBuffer);
	mColorAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
	mColorAttribute->setVertexSize(VTX_COLOR_SIZE);
	mColorAttribute->setByteOffset(VTX_POS_SIZE * sizeof(VTX_TYPE));
	mColorAttribute->setByteStride(VTX_STRIDE);
	//mColorAttribute->setCount(numVerticies);
	mColorAttribute->setName(Qt3DRender::QAttribute::defaultColorAttributeName());

//	mIndexAttribute = new Qt3DRender::QAttribute(this);
//	mIndexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
//	//mIndexAttribute->setBuffer(indexBuffer);
//	mIndexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
//	mIndexAttribute->setVertexSize(1);
//	mIndexAttribute->setByteOffset(0);
//	mIndexAttribute->setByteStride(0);
	//mIndexAttribute->setCount(indexDataIdx);

	loadMesh(dataset);

	addAttribute(mPosAttribute);
	addAttribute(mColorAttribute);
//	addAttribute(mIndexAttribute);
}

SHPMeshGeometry::~SHPMeshGeometry()
{

}

// generally used geo measurement function
static double measure(float lat1, float lon1, float lat2, float lon2)
{
	double R = 6378.137; // Radius of earth in KM
	double dLat = lat2 * M_PI / 180 - lat1 * M_PI / 180;
	double dLon = lon2 * M_PI / 180 - lon1 * M_PI / 180;
	double a = std::sin(dLat/2)
			* std::sin(dLat/2)
			+ std::cos(lat1 * M_PI / 180)
			* std::cos(lat2 * M_PI / 180)
			* std::sin(dLon/2)
			* std::sin(dLon/2);

	double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1-a));
	double d = R * c;
	return d * 1000; // meters
}

int countPoints(OGRGeometry *geom)
{
	OGRCurve *curve = dynamic_cast<OGRCurve*>(geom);

	if (curve != nullptr)
	{
		return curve->getNumPoints();
	}

	OGRGeometryCollection *collection = dynamic_cast<OGRGeometryCollection*>(geom);
	if (collection != nullptr)
	{
		int numGeom = collection->getNumGeometries();

		int numPoints = 0;
		for (int i = 0; i < numGeom; ++i)
		{
			numPoints += countPoints(collection->getGeometryRef(i));
		}

		return numPoints;
	}

	OGRCurvePolygon *curvePolygon = dynamic_cast<OGRCurvePolygon*>(geom);
	if (curvePolygon != nullptr)
	{
		int numIntGeom = curvePolygon->getNumInteriorRings();
		int numGeom = numIntGeom + 1;

		int numPoints = countPoints(curvePolygon->getExteriorRingCurve());

		for (int i = 0; i < numIntGeom; ++i)
		{
			OGRCurve *curve = curvePolygon->getInteriorRingCurve(i);
			numPoints += countPoints(curve);
		}

		return numPoints;
	}

	qDebug() << "ERROR: UNKNOWN GEOMETRY: " << geom->getGeometryName();
	exit(1);
}

void reportGeometry(const QString &prefix, OGRGeometry *geom)
{
	OGRCurve *curve = dynamic_cast<OGRCurve*>(geom);

	if (curve != nullptr)
	{
		printf("%s%s (%d points)\n", prefix.toLatin1().data(), curve->getGeometryName(), curve->getNumPoints());
		return;
	}

	OGRGeometryCollection *collection = dynamic_cast<OGRGeometryCollection*>(geom);
	if (collection != nullptr)
	{
		int numGeom = collection->getNumGeometries();
		printf("%s%s (%d geoms)\n", prefix.toLatin1().data(), collection->getGeometryName(), numGeom);

		for (int i = 0; i < numGeom; ++i)
		{
			reportGeometry(prefix + "  ", collection->getGeometryRef(i));
		}

		return;
	}

	OGRCurvePolygon *curvePolygon = dynamic_cast<OGRCurvePolygon*>(geom);
	if (curvePolygon != nullptr)
	{
		int numIntGeom = curvePolygon->getNumInteriorRings();
		int numGeom = numIntGeom + 1;

		printf("%s%s (%d rings)\n", prefix.toLatin1().data(), curvePolygon->getGeometryName(), numGeom);

		reportGeometry(prefix + "  ", curvePolygon->getExteriorRingCurve());

		for (int i = 0; i < numIntGeom; ++i)
		{
			OGRCurve *curve = curvePolygon->getInteriorRingCurve(i);
			reportGeometry(prefix + "  ", curve);
		}

		return;
	}

	qDebug() << "ERROR: UNKNOWN GEOMETRY: " << geom->getGeometryName();
	exit(1);
}

void SHPMeshGeometry::loadMesh(GDALDataset *dataset)
{
	int numLayers = dataset->GetLayerCount();

	OGRSpatialReference sourceSrs(dataset->GetProjectionRef());

	OGRSpatialReference wgsSrs;
	wgsSrs.SetProjCS("UTM 11 / WGS84");
	wgsSrs.SetWellKnownGeogCS("WGS84");
	wgsSrs.SetUTM(11);

	OGRSpatialReference mercSrs;
	OGRErr retr = mercSrs.importFromEPSG(3776);
	//mercSrs.SetProjCS("Mercator Srs");
//	OGRErr ret = mercSrs.SetWellKnownGeogCS("EPSG:3857");
	if (retr != OGRERR_NONE)
	{
		qDebug() << " couldnt get well known CS for transform... ";
		return;
	}
//	mercSrs.SetUTM(11);

	qDebug() << " num layers: " << numLayers << " projRef: " << dataset->GetProjectionRef();

	if (numLayers < 1)
	{
		qDebug() << "ERROR: no layers?!?!";
		return;
	}

	int numPoints = 0;
	for (auto layer: dataset->GetLayers())
	{
		layer->ResetReading();
		layer->SetSpatialFilter(nullptr);

		OGRFeatureDefn *layerDefn = layer->GetLayerDefn();

		int layerDefnFieldCount = layerDefn->GetFieldCount();

		printf("Layer: %s (%s)\n", layer->GetName(), layerDefn->GetName());

		qDebug() << " geom field count: " << layerDefn->GetGeomFieldCount();

		for( int iAttr = 0; iAttr < layerDefn->GetFieldCount(); iAttr++ )
		{
			OGRFieldDefn *poField = layerDefn->GetFieldDefn(iAttr);
			const char* pszType = (poField->GetSubType() != OFSTNone)
										 ? CPLSPrintf(
							"%s(%s)",
							poField->GetFieldTypeName(poField->GetType()),
							poField->GetFieldSubTypeName(poField->GetSubType()))
										 : poField->GetFieldTypeName(poField->GetType());
			printf("  %s: %s (%d.%d)",
					 poField->GetNameRef(),
					 pszType,
					 poField->GetWidth(),
					 poField->GetPrecision());
			if( !poField->IsNullable() )
				printf(" NOT NULL");
			if( poField->GetDefault() != nullptr )
				printf(" DEFAULT %s", poField->GetDefault());
			printf("\n");
		}

		const int geomFieldCount =
				layer->GetLayerDefn()->GetGeomFieldCount();
		if( geomFieldCount > 1 )
		{
			for(int iGeom = 0;iGeom < geomFieldCount; iGeom ++ )
			{
				OGRGeomFieldDefn* poGFldDefn =
						layer->GetLayerDefn()->GetGeomFieldDefn(iGeom);
				printf("Geometry (%s): %s\n", poGFldDefn->GetNameRef(),
						 OGRGeometryTypeToName(poGFldDefn->GetType()));
			}
		}
		else
		{
			printf(" Geometry: %s\n",
					 OGRGeometryTypeToName(layer->GetGeomType()));
		}

		for(int iGeom = 0;iGeom < geomFieldCount; iGeom ++ )
		{
			OGRGeomFieldDefn* poGFldDefn =
					layer->GetLayerDefn()->GetGeomFieldDefn(iGeom);
			if( geomFieldCount == 1 &&
				 EQUAL(poGFldDefn->GetNameRef(), "") &&
				 poGFldDefn->IsNullable() )
				break;
			printf("Geometry Column ");
			if( geomFieldCount > 1 )
				printf("%d ", iGeom + 1);
			if( !poGFldDefn->IsNullable() )
				printf("NOT NULL ");
			printf("= %s\n", poGFldDefn->GetNameRef());
		}

		OGRSpatialReference *layerSpatialRef = layer->GetSpatialRef();

		OGRCoordinateTransformation *coordTransform = OGRCreateCoordinateTransformation(layerSpatialRef, &mercSrs);

		OGRwkbGeometryType origGeomType = layer->GetGeomType();
		OGRwkbGeometryType geometryType = wkbFlatten(origGeomType);

		qDebug() << " Flattened Type: " << OGRGeometryTypeToName(geometryType);

		OGREnvelope oExt;
		if( geomFieldCount > 1 )
		{
			for( int iGeom = 0;iGeom < geomFieldCount; iGeom ++ )
			{
				if( layer->GetExtent(iGeom, &oExt, TRUE) == OGRERR_NONE )
				{
					OGRGeomFieldDefn* poGFldDefn =
							layer->GetLayerDefn()->GetGeomFieldDefn(iGeom);
					CPLprintf(" Extent (%s): (%f, %f) - (%f, %f)\n",
								 poGFldDefn->GetNameRef(),
								 oExt.MinX, oExt.MinY, oExt.MaxX, oExt.MaxY);
				}
			}
		}
		else if( layer->GetExtent(&oExt, TRUE) == OGRERR_NONE )
		{
			CPLprintf(" Extent: (%f, %f) - (%f, %f)\n",
						 oExt.MinX, oExt.MinY, oExt.MaxX, oExt.MaxY);
		}

		OGREnvelope envelope;
		OGRErr ret = layer->GetExtent(&envelope);
		if (ret != OGRERR_NONE)
		{
			qDebug() << "ERROR: failed to get extent!";
			return;
		}

		qDebug() << " Extent: "
			<< envelope.MinX << ", " << envelope.MinY << ", "
			<< envelope.MaxX << ", " << envelope.MaxY;

		double extentX[2] = { envelope.MinX, envelope.MaxX };
		double extentY[2] = { envelope.MinY, envelope.MaxY };
		double extentZ[2] = { 0.0f, 0.0f };
		coordTransform->Transform(2, extentX, extentY, extentZ );

		double width = extentX[1] - extentX[0];
		double height = extentY[1] - extentY[0];

		GeometryBuilder geometryBuilder(extentX[0], extentY[0], width, height);

		mOriginX = extentX[0];
		mOriginY = extentY[0];

		qDebug() << " extentc: " << extentX[0] << ", " << extentY[0] << ", " << extentX[1] << ", " << extentY[1];

		qDebug() << " size: " << extentX[1] - extentX[0] << ", " << extentY[1] - extentY[0];

		qDebug() << " rasters: " << dataset->GetRasterCount();

//		int featCount = 0;
//
//		for (auto &feat: dataset->GetFeatures())
//		{
//			featCount++;
//		}
//
//		qDebug() << " features: " << featCount;


		qDebug() << " num features: " << layer->GetFeatureCount(1);

		char *pszWKT;
		if (layerSpatialRef == NULL)
			pszWKT = CPLStrdup("(unknown)");
		else
		{
			layerSpatialRef->exportToPrettyWkt(&pszWKT);
		}

		printf(" SRS WKT:\n%s\n", pszWKT);
		CPLFree(pszWKT);

		if (strlen(layer->GetFIDColumn()) > 0)
			printf("FID Column = %s\n",
					 layer->GetFIDColumn());

		if (strlen(layer->GetGeometryColumn()) > 0)
			printf("Geometry Column = %s\n",
					 layer->GetGeometryColumn());

		layer->ResetReading();
		OGRFeature *poFeature = nullptr;
		int countedFeatures = 0;

		/*
  addr_rg_l: String (254.0)
  addr_rg_r: String (254.0)
  class_cd: String (254.0)
  class_de: String (254.0)
  digitiz_cd: String (254.0)
  digitz_de: String (254.0)
  id: Real (33.31)
  name: String (254.0)
  name_ab: String (254.0)
  name_ab_pr: String (254.0)
  name_pr: String (254.0)
  resp_cd: String (254.0)
  resp_de: String (254.0)
  type_cd: String (254.0)
  type_de: String (254.0)


		 */
//		int classcd_id = layerDefn->GetFieldIndex("class_cd");
//		int classde_id = layerDefn->GetFieldIndex("class_de");
//		int digitiz_id = layerDefn->GetFieldIndex("digitiz_cd");
//		int digitzde_id = layerDefn->GetFieldIndex("digitz_de");
//
//		std::set<std::string> uniqueClassCD;
//		std::set<std::string> uniqueClassDE;
//		std::set<std::string> uniqueDigitiz;
//		std::set<std::string> uniqueDigitzDE;

		while ((poFeature = layer->GetNextFeature()))
		{
//			const char *class_cd = poFeature->GetFieldAsString(classcd_id);
//			const char *class_de = poFeature->GetFieldAsString(classde_id);
//			const char *digitiz = poFeature->GetFieldAsString(digitiz_id);
//			const char *digitz_de = poFeature->GetFieldAsString(digitzde_id);
//
//			uniqueClassCD.insert(class_cd);
//			uniqueClassDE.insert(class_de);
//			uniqueDigitiz.insert(digitiz);
//			uniqueDigitzDE.insert(digitz_de);

			if (poFeature->GetGeomFieldCount() != 1)
			{
				qDebug() << " feature geom field cnt: " << poFeature->GetGeomFieldCount();
			}

			OGRGeometry *ref = poFeature->GetGeomFieldRef(0);
			if (ref == nullptr)
			{
				qDebug() << " geom field ref == null";
			}

			OGRGeometry *geometry = poFeature->GetGeometryRef();
			if (geometry == nullptr)
			{
				qDebug() << " ERROR no geometry? ERROR ";
				continue;
			}

			if (ref == geometry)
			{
				//qDebug() << " geomfieldref == geometry";
			}
			else
			{
				qDebug() << " geomfieldref != geometry";
			}

			//poFeature->DumpReadable(nullptr, nullptr);

			countedFeatures++;

			//OGRSpatialReference *spatialRef = geometry->getSpatialReference();
			//qDebug() << " spacialRef: " << spatialRef->GetName();

			geometry->transform(coordTransform);

			//OGRGeometry *polygonized = geometry->Polygonize();

			//qDebug() << " type: " << geometry->getGeometryName();

			OGRwkbGeometryType type = geometry->getGeometryType();
			OGRwkbGeometryType flattenedType = wkbFlatten(type);

//			if (type != flattenedType)
//			{
//				printf(" type(%s) != flattenedType(%s)\n", OGRGeometryTypeToName(type), OGRGeometryTypeToName(flattenedType));
//			}

			numPoints += countPoints(geometry);

			QVector4D color(0.0f, 0.0f, 0.0f, 1.0f);

			if (poFeature->GetFID() == 32675) {
				qDebug("got magic thing:");
				poFeature->DumpReadable(nullptr, nullptr);
				geometry->dumpReadable(nullptr, nullptr);
				color = QVector4D(1.0f, 0.0f, 0.0f, 1.0f);
			}

			if (type == wkbLineString)
			{
				geometryBuilder.addLineString(geometry->toLineString(), color);
			}
			else if (type == wkbMultiLineString || type == wkbMultiLineString25D)
			{
				qDebug() << " multiline string!";
			}
			else if (type == wkbPolygon || type == wkbPolygon25D)
			{
				OGRPolygon *polygon = geometry->toPolygon();
				if (polygon->getExteriorRing() == nullptr)
				{
					qDebug() << " ERROR: polygon with no exterior ring??";
				}

				geometryBuilder.addPolygon(polygon);
			}
			else if (type == wkbMultiPolygon)
			{
				OGRMultiPolygon *multiPolygon = geometry->toMultiPolygon();
				int numGeom = multiPolygon->getNumGeometries();
				if (numGeom < 1)
				{
					qDebug() << " ERROR: multi polygon with no polygons?";
				}

//			for (int i = 0; i < numGeom; ++i)
//			{
//				OGRGeometry *poly = multiPolygon->getGeometryRef(i);
//				if (poly->getGeometryType() != wkbPolygon && poly->getGeometryType() != wkbPolygon25D)
//				{
//					qDebug() << " UNSUPPORTED GEOM TYPE FOR MULTIPOLY: " << poly->getGeometryName();
//				}
//				geometryBuilder.addPolygon(poly->toPolygon());
//			}

				for (auto &geom: *multiPolygon)
				{
					if (geom->getGeometryType() != wkbPolygon && geom->getGeometryType() != wkbPolygon25D)
					{
						qDebug() << " UNSUPPORTED GEOM TYPE FOR MULTIPOLY: " << geom->getGeometryName();
					}
					geometryBuilder.addPolygon(geom->toPolygon());
				}

			}
			else
			{
				qDebug() << " UNSUPPORTED GEOMETRY TYPE >:(" << geometry->getGeometryName() << "("
							<< geometry->getGeometryType() << ")";
			}
		}

		printf(" counted features: %d\n", countedFeatures);

//		if (featCount != countedFeatures)
//		{
//			printf("featCount(%d) != countedFeatures(%d)!\n", featCount, countedFeatures);
//		}

		qDebug() << " geom bounds: " << geometryBuilder.getMinPos() << ", " << geometryBuilder.getMaxPos();

		qDebug() << " counted points: " << numPoints << " builder points: " << geometryBuilder.getNumVerticies();

		int expectedIndexes = numPoints + countedFeatures;

		qDebug() << " expected indexes: " << (expectedIndexes) << " builder indexes: " << geometryBuilder.getNumIndicies();

//		QString layerFileName = QString(layer->GetName()) + ".png";
//		QFileInfo fileInfo = QFileInfo(path);
//		fileInfo.setFile(layerFileName);

		//QPixmap *pixmap = geometryBuilder.getPixmap();
		//qDebug() << " saving (" << pixmap->width() << "," << pixmap->height() << ") pixmap to: " << fileInfo;
		//pixmap->save(fileInfo.filePath());

//		fileInfo.refresh();

//		qDebug() << " saved pixmap to: " << fileInfo;

		mVertexBuffer->setData(geometryBuilder.getVerticies());
		mIndexBuffer->setData(geometryBuilder.getIndicies());

		mPosAttribute->setBuffer(mVertexBuffer);
		mPosAttribute->setCount(geometryBuilder.getNumVerticies());

		mColorAttribute->setBuffer(mVertexBuffer);
		mColorAttribute->setCount(geometryBuilder.getNumVerticies());

		mNumVerticies = geometryBuilder.getNumVerticies();

		//mIndexAttribute->setBuffer(mIndexBuffer);
		//mIndexAttribute->setCount(geometryBuilder.getNumIndicies());

//		qDebug() << " unique class_cd values: ";
//		for (auto &it: uniqueClassCD)
//		{
//			std::cout << "   " << it << std::endl;
//		}
//
//		qDebug() << " unique class_de values: ";
//		for (auto &it: uniqueClassDE)
//		{
//			std::cout << "   " << it << std::endl;
//		}
//
//		qDebug() << " unique digitz values: ";
//		for (auto &it: uniqueDigitiz)
//		{
//			std::cout << "   " << it << std::endl;
//		}
//
//		qDebug() << " unique digitzDE values: ";
//		for (auto &it: uniqueDigitzDE)
//		{
//			std::cout << "   " << it << std::endl;
//		}

		qDebug() << "done with layer.";
	}

	qDebug() << "ASDF!";
}

