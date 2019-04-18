//
// Created by moose on 05/04/19.
//

#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QAttribute>
#include <GeometryBuilder.h>
#include <src/LogUtils.h>
#include "Constants.h"
#include "QuadTreeNodeGeometry.h"
#include "QuadTreeNodeGeometryGenerator.h"
#include "QuadTreeNodeItem.h"

using namespace Qt3DRender;

#define TAG "QuadTreeNodeGeometry"

QuadTreeNodeGeometry::QuadTreeNodeGeometry(QuadTreeNode *node, Qt3DCore::QNode *parent)
	: QGeometry(parent), mNode{node}, mNumVerticies{0}
{
	log_trace_enter();

	mVertexBuffer = new QBuffer(Qt3DRender::QBuffer::VertexBuffer, this);
	mVertexBuffer->setAccessType(QBuffer::ReadWrite);
	mVertexBuffer->setUsage(QBuffer::DynamicDraw);
	mVertexBuffer->setDataGenerator(QSharedPointer<QuadTreeNodeGeometryGenerator>::create(this));

	//mIndexBuffer = new QBuffer(Qt3DRender::QBuffer::IndexBuffer, this);

	mPosAttribute = new QAttribute(this);
	mPosAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
	//posAttribute->setBuffer(vertexBuffer);
	mPosAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
	mPosAttribute->setVertexSize(VTX_POS_SIZE);
	mPosAttribute->setByteOffset(0);
	mPosAttribute->setByteStride(VTX_STRIDE);
	//posAttribute->setCount(numVerticies);
	mPosAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

	mColorAttribute = new QAttribute(this);
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

	addAttribute(mPosAttribute);
	addAttribute(mColorAttribute);
//	addAttribute(mIndexAttribute);

	log_trace_exit();
}

QuadTreeNodeGeometry::~QuadTreeNodeGeometry()
{
	// empty
	//log_trace_exit();
}

QByteArray QuadTreeNodeGeometry::generateVertexData()
{
	log_trace_enter();

	if (mNode->haveItems())
	{
		GeometryBuilder builder(bounds());

		for (QuadTreeNodeItem *item: mNode->getItems())
		{
			item->buildGeometry(&builder);
		}

		mNumVerticies = builder.getNumVerticies();

		//mPosAttribute->setCount(mNumVerticies);
		//mColorAttribute->setCount(mNumVerticies);


		log_debug("%s items %s verts", mNode->getItemCount(), mNumVerticies);

		log_trace_exit();

		return builder.getVerticies();
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