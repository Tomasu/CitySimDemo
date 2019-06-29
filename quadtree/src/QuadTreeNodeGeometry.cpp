//
// Created by moose on 05/04/19.
//


#include "core/GeometryBuilder.h"
#include "core/Constants.h"
#include "quadtree/QuadTreeNodeGeometry.h"
#include "quadtree/QuadTreeNodeGeometryGenerator.h"
#include "quadtree/QuadTreeNodeItem.h"
#include "quadtree/QuadTreeNode.h"
#include "core/Line.h"

#include <stdexcept>

#include <QColor>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QAttribute>
#include <QMutexLocker>


using namespace Qt3DRender;

#include "util/LogUtils.h"
#define TAG "QuadTreeNodeGeometry"

QuadTreeNodeGeometry::QuadTreeNodeGeometry(QuadTreeNode *node, Qt3DCore::QNode *parent)
	: QGeometry(parent), mNode{node}, mNumVerticies{0},
	mPosAttribute{nullptr}, mNormalAttribute{nullptr},
	mColorAttribute{nullptr},
	mVertexBuffer{nullptr}, mIndexBuffer{nullptr}
{
	sId++;

	mId = sId;
	log_debug("mId=%s", mId);

	log_trace_enter();

	initialize();

	log_trace_exit();
}

void QuadTreeNodeGeometry::initialize()
{
	log_trace_enter();

	if (mVertexBuffer != nullptr
		|| mPosAttribute != nullptr
		|| mNormalAttribute != nullptr
		|| mColorAttribute != nullptr)
	{
		log_debug("already initialized?!");
		throw std::runtime_error("already initialized");
	}

	mNumVerticies = 0;

	mVertexBuffer = new QBuffer(QBuffer::VertexBuffer, this);
	mVertexBuffer->setAccessType(QBuffer::ReadWrite);
	mVertexBuffer->setUsage(QBuffer::DynamicDraw);
	//mVertexBuffer->setDataGenerator(QSharedPointer<QuadTreeNodeGeometryGenerator>::create(this));


	//mIndexBuffer = new QBuffer(QBuffer::IndexBuffer, this);

	mPosAttribute = new QAttribute(this);
	mPosAttribute->setAttributeType(QAttribute::VertexAttribute);
	mPosAttribute->setBuffer(mVertexBuffer);
	//posAttribute->setBuffer(vertexBuffer);
	mPosAttribute->setVertexBaseType(QAttribute::Float);
	mPosAttribute->setVertexSize(VTX_POS_SIZE);
	mPosAttribute->setByteOffset(VTX_POS_OFFSET);
	mPosAttribute->setByteStride(VTX_STRIDE);
	//posAttribute->setCount(numVerticies);
	mPosAttribute->setName(QAttribute::defaultPositionAttributeName());

// 	mNormalAttribute = new QAttribute(this);
// 	mNormalAttribute->setAttributeType(QAttribute::VertexAttribute);
// 	mNormalAttribute->setBuffer(mVertexBuffer);
// 	mNormalAttribute->setVertexBaseType(QAttribute::Float);
// 	mNormalAttribute->setVertexSize(VTX_NORM_SIZE);
// 	mNormalAttribute->setByteOffset(VTX_NORM_OFFSET);
// 	mNormalAttribute->setByteStride(VTX_STRIDE);
// 	mNormalAttribute->setName(QAttribute::defaultNormalAttributeName());

	mColorAttribute = new QAttribute(this);
	mColorAttribute->setAttributeType(QAttribute::VertexAttribute);
	mColorAttribute->setBuffer(mVertexBuffer);
	//mColorAttribute->setBuffer(vertexBuffer);
	mColorAttribute->setVertexBaseType(QAttribute::Float);
	mColorAttribute->setVertexSize(VTX_COLOR_SIZE);
	mColorAttribute->setByteOffset(VTX_COLOR_OFFSET);
	mColorAttribute->setByteStride(VTX_STRIDE);
	//mColorAttribute->setCount(numVerticies);
	mColorAttribute->setName(QAttribute::defaultColorAttributeName());

	//	mIndexAttribute = new QAttribute(this);
	//	mIndexAttribute->setAttributeType(QAttribute::IndexAttribute);
	//	//mIndexAttribute->setBuffer(indexBuffer);
	//	mIndexAttribute->setVertexBaseType(QAttribute::UnsignedInt);
	//	mIndexAttribute->setVertexSize(1);
	//	mIndexAttribute->setByteOffset(0);
	//	mIndexAttribute->setByteStride(0);
	//mIndexAttribute->setCount(indexDataIdx);

	generateVertexData();

		addAttribute(mPosAttribute);
	//	addAttribute(mNormalAttribute);
		addAttribute(mColorAttribute);
	////	addAttribute(mIndexAttribute);

	//mVertexBuffer->setSyncData(true);


	log_trace_exit();
}

QuadTreeNodeGeometry::~QuadTreeNodeGeometry()
{
	// empty
	//log_trace_exit();
}

void QuadTreeNodeGeometry::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change)
{
	QGeometry::sceneChangeEvent(change);

	log_debug("geom change: %s", change);
}

QByteArray QuadTreeNodeGeometry::generateVertexData()
{
	//QMutexLocker locker(&sMutex);

	log_trace_enter();

	//removeAttribute(mPosAttribute);
	//removeAttribute(mColorAttribute);

//  	if (mPosAttribute != nullptr)
//  	{
//  		removeAttribute(mPosAttribute);
//  		mPosAttribute = nullptr;
//  	}
//
//  	if (mColorAttribute != nullptr)
//  	{
//  		removeAttribute(mColorAttribute);
//  		mColorAttribute = nullptr;
//  	}

	if (mNode->haveItems())
	{
		//QVector<QPointF> line;
		//line.append({0,0,0});

		// get root node..
// 		QuadTreeNode *ptr = mNode;
// 		while(ptr->getParent() != nullptr)
// 		{
// 			ptr = ptr->getParent();
// 		}

		Point topLeft({0,0,0});
		Point reposBr = Point(bounds().bottomRight() - bounds().topLeft());
		Point bottomRight(reposBr.x(), reposBr.y(), 0.0f);

		Size size = bounds().size();
		Rect boundsOffset = bounds();
		//boundsOffset.moveTopLeft({0.0f, 0.0f});


		GeometryBuilder builder(bounds(), BASE_Z_OFFSET);

		QColor hslColor = QColor::fromHsl(360.0f / (float)sId * mId, 255, 128);
		log_debug("hslColor: h=%s s=%s l=%s", hslColor.hslHue(), hslColor.hslSaturation(), hslColor.lightness());

		float cc1 = (1.0f / (sId+4)) * mId + 0.1f;
		log_debug("%s cc: %s", mId, cc1);
		QColor roadColor = QColor::fromRgbF(cc1,cc1,cc1);
		builder.setColor(hslColor/*roadColor*/);

		//QVector4D gridColor = QVector4D::
		float cc = 0.8;//(mId * 16.0f + 64.0f) / 256.0f;
		QColor gridColor = QColor::fromRgbF(0.5f, cc, 0.0f, 1.0f);
		//if (!mNode->hasParent())
		{
			Line leftLine = { boundsOffset.topLeft(), boundsOffset.bottomLeft() };
			builder.addLine(leftLine, gridColor);

			Line topLine = { boundsOffset.topLeft(), boundsOffset.topRight() };
			builder.addLine(topLine, gridColor);

			Line rightLine = { boundsOffset.topRight(), boundsOffset.bottomRight() };
			builder.addLine(rightLine, gridColor);

			Line bottomLine = { boundsOffset.bottomLeft(), boundsOffset.bottomRight() };
			builder.addLine(bottomLine, gridColor);
		}

		//builder.addLineString({start, end});

		//log_debug(" add line: %s - %s", start, end);

		for (QuadTreeNodeItem *item: mNode->getItems())
		{
			item->buildGeometry(&builder);
		}

		mNumVerticies = builder.getNumVerticies();

		//mPosAttribute->setCount(mNumVerticies);
		//mColorAttribute->setCount(mNumVerticies);


		log_debug("%s items %s verts", mNode->getItemCount(), mNumVerticies);

		const QByteArray &data = builder.getVerticies();

		float *dptr = (float*)data.constData();
		size_t num_floats = data.size() / sizeof(float);

		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::min();
		float minY = std::numeric_limits<float>::max();
		float maxY = std::numeric_limits<float>::min();
		for(size_t i = 0; i < num_floats; i+= VTX_STRIDE)
		{
			float x = dptr[i] + boundsOffset.left(), y = dptr[i+1] + boundsOffset.top();

			if (x < minX)
			{
				minX = x;
			}

			if (x > maxX)
			{
				maxX = x;
			}

			if (y < minY)
			{
				minY = y;
			}

			if (y > maxY)
			{
				maxY = y;
			}
		}

		log_debug("geom calc bounds: %s,%s %sx%s", minX, minY, maxX - minX, maxY - minY);
		log_debug("node bounds: %s", mNode->bounds());

		mVertexBuffer->setData(data);

		mPosAttribute->setCount(mNumVerticies);
		mColorAttribute->setCount(mNumVerticies);
	//	mNormalAttribute->setCount(mNumVerticies);

		//addAttribute(mPosAttribute);
		//addAttribute(mColorAttribute);
		//	addAttribute(mIndexAttribute);

		//initialize();

		log_trace_exit();

		return data;
	}


	log_debug("no items?!");

	log_trace_exit();

	return {};
}

bool QuadTreeNodeGeometry::operator==(const QuadTreeNodeGeometry &other) const
{
	bool test = other.mNode != nullptr
		&& other.mNode == mNode
		&& other.getNumVerticies() == getNumVerticies()
		&& other.mVertexBuffer->data().size() == mVertexBuffer->data().size()
		// TODO: other.mIndexBuffer == blah
		;

	log_debug("test=%s", test);

	return test;
}

const Rect& QuadTreeNodeGeometry::bounds() const{ return mNode->bounds(); }

int QuadTreeNodeGeometry::sId = 0;
QMutex QuadTreeNodeGeometry::sMutex;
