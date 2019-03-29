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
#include "ShpMeshGeometry.h"
#include "LocUtil.h"

using namespace Qt3DRender;
using namespace Qt3DCore;

SHPMeshGeometry::SHPMeshGeometry(const QString &path, QNode *parent) : QGeometry(parent)
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
	mPosAttribute->setByteStride(ELEMENT_STRIDE);
	//posAttribute->setCount(numVerticies);
	mPosAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

	mColorAttribute = new Qt3DRender::QAttribute(this);
	mColorAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
	//mColorAttribute->setBuffer(vertexBuffer);
	mColorAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
	mColorAttribute->setVertexSize(VTX_COLOR_SIZE);
	mColorAttribute->setByteOffset(VTX_POS_SIZE * sizeof(float));
	mColorAttribute->setByteStride(ELEMENT_STRIDE);
	//mColorAttribute->setCount(numVerticies);
	mColorAttribute->setName(Qt3DRender::QAttribute::defaultColorAttributeName());

	mIndexAttribute = new Qt3DRender::QAttribute(this);
	mIndexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
	//mIndexAttribute->setBuffer(indexBuffer);
	mIndexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
	mIndexAttribute->setVertexSize(1);
	mIndexAttribute->setByteOffset(0);
	mIndexAttribute->setByteStride(0);
	//mIndexAttribute->setCount(indexDataIdx);

	loadMesh(path);

	addAttribute(mPosAttribute);
	addAttribute(mColorAttribute);
	addAttribute(mIndexAttribute);
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

void SHPMeshGeometry::loadMesh(const QString &path)
{
	SHPHandle shpHandle;

	shpHandle = SHPOpen(path.toLatin1().data(), "rb");

	int numEntities = 0;
	int shapeType = -1;
	double padMinBound[4];
	double padMaxBound[4];

	SHPGetInfo(shpHandle, &numEntities, &shapeType, padMinBound, padMaxBound);

	qDebug() << "loaded " << path << " with " << numEntities << " entities and type " << SHPTypeName(shapeType);
	qDebug() << " bounds min: " << shpHandle->adBoundsMin[0] << ", " << shpHandle->adBoundsMin[1] << ", " << shpHandle->adBoundsMin[2];
	qDebug() << " bounds max: " << shpHandle->adBoundsMax[0] << ", " << shpHandle->adBoundsMax[1] << ", " << shpHandle->adBoundsMax[2];

	QVector3D origin = DegToCartesian(padMinBound[1], padMinBound[0]);
	QVector3D originMax = DegToCartesian(padMaxBound[1], padMaxBound[0]);

	QVector3D size = originMax - origin;

	std::cout << std::setprecision(10) << "origin: " << origin.x() << ", " << origin.y()
		<< " max: " << originMax.x() << ", " << originMax.y()
		<< " width: " << size.x() << " height: " << size.y()
		<< std::endl;

	SHPObject *obj = SHPReadObject(shpHandle, 0);
	if (obj != nullptr)
	{
		qDebug() << "obj: type= " << SHPTypeName(obj->nSHPType) << " verts=" << obj->nVertices;// << obj->padfX[0];

		for (int i = 0; i < obj->nParts; i++)
		{
			int ptype = obj->panPartType[i];
			int pstart = obj->panPartStart[i];

			qDebug() << " part " << SHPPartTypeName(ptype) << " start=" << pstart;

			for (int j = pstart; j < obj->nVertices; j++)
			{
				QVector3D pos = DegToCartesian(obj->padfY[i], obj->padfX[i]);
				//double pos[2] = { 0.0f, 0.0f };
				//LatLonToUTMXY(DegToRad(obj->padfX[i]), DegToRad(obj->padfY[i]), zone, pos);

				QVector3D diff = pos - origin;

				//double x = (obj->padfX[j] - padMinBound[0])*10;
				//double y = (obj->padfY[j] - padMinBound[1])*10;
				//double z = (obj->padfZ[j] - padMinBound[2])*10;
				std::cout << std::setprecision(10) << " pt vtx (" << pos.x() << ", " << pos.y() << ") " << " conv (" << diff.x() << ", " << diff.y() /* << ", " << z */ << ")" << std::endl;
			}
		}
	}

	int numVerticies = 0;
	int numIndicies = 0;
	for (int i = 0; i < numEntities; i++)
	{
		SHPObject *obj = SHPReadObject(shpHandle, i);
		numVerticies += obj->nVertices; // line segments. share verticies.
		numIndicies += (obj->nVertices * 2); //+ obj->nParts; // double num verticies
	}

	QByteArray vertexBufferData;
	// position and color (and normals...)
	vertexBufferData.resize(numVerticies * (VTX_POS_SIZE + VTX_COLOR_SIZE /*+ 3*/) * sizeof(float));

	float *rawVertexArray = reinterpret_cast<float*>(vertexBufferData.data());
	int idx = 0;

	QByteArray indexBufferData;
	indexBufferData.resize(numIndicies * sizeof(uint));

	uint *rawIndexArray = reinterpret_cast<uint *>(indexBufferData.data());
	int indexDataIdx = 0;

	srand(time(nullptr));

	int curVertex = 0;

	for (int i = 0; i < numEntities; i++)
	{
		SHPObject *obj = SHPReadObject(shpHandle, i);

		float redComp = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float greenComp = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float blueComp = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		QVector3D entityColor(redComp, greenComp, blueComp);

		if (obj != nullptr)
		{
			//qDebug() << "obj: type= " << SHPTypeName(obj->nSHPType) << " verts=" << obj->nVertices;// << obj->padfX[0];

			int curIdxStart = curVertex;

			for (int i = 0; i < obj->nVertices; i++)
			{
				QVector3D pos = DegToCartesian(obj->padfY[i], obj->padfX[i]);
				QVector3D diff = pos - origin;

				rawVertexArray[idx++] = diff.x(); // / 10.0f;
				rawVertexArray[idx++] = diff.y(); // / 10.0f;
				rawVertexArray[idx++] = 0.0f;

//				rawVertexArray[idx++] = (obj->padfX[i] - padMinBound[0]) * 1000;
//				rawVertexArray[idx++] = (obj->padfY[i] - padMinBound[1]) * 1000;
//				rawVertexArray[idx++] = (obj->padfZ[i] - padMinBound[2]) * 1000;

				rawVertexArray[idx++] = entityColor.x();
				rawVertexArray[idx++] = entityColor.y();
				rawVertexArray[idx++] = entityColor.z();

				curVertex++;
			}

			for (int i = 0; i < obj->nParts; i++)
			{
				int ptype = obj->panPartType[i];
				int pstart = obj->panPartStart[i];

				//qDebug() << " part " << SHPPartTypeName(ptype) << " start=" << pstart;

				for (int j = pstart; j < obj->nVertices-1; j++)
				{
					rawIndexArray[indexDataIdx++] = j + curIdxStart;
					rawIndexArray[indexDataIdx++] = j + curIdxStart + 1;

//					float x = (obj->padfX[j] - padMinBound[0]) * 100;
//					float y = (obj->padfY[j] - padMinBound[1]) * 100;
//					float z = (obj->padfZ[j] - padMinBound[2]) * 100;

					//qDebug() << " pt (" << x << ", " << y << ", " << z << ")";
				}

				//rawIndexArray[indexDataIdx++] = 65535;
			}
		}
	}

	mVertexBuffer->setData(vertexBufferData);
	mIndexBuffer->setData(indexBufferData);

	mPosAttribute->setBuffer(mVertexBuffer);
	mPosAttribute->setCount(numVerticies);

	mColorAttribute->setBuffer(mVertexBuffer);
	mColorAttribute->setCount(numVerticies);

	mNumVerticies = numVerticies;

	mIndexAttribute->setBuffer(mIndexBuffer);
	mIndexAttribute->setCount(numIndicies);

	qDebug() << "ASDF!";
}

