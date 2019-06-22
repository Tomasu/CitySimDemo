//
// Created by moose on 05/04/19.
//

#include <Qt3DCore/QPropertyUpdatedChange>
#include "QuadTreeNodeGeometryRenderer.h"
#include "QuadTreeNodeGeometry.h"
#include "QuadTreeNodeGeometryFactory.h"

#include "LogUtils.h"
#define TAG "QuadTreeNodeGeometryRenderer"

QuadTreeNodeGeometryRenderer::QuadTreeNodeGeometryRenderer(QuadTreeNodeGeometryFactory *geometryFactory, Qt3DCore::QNode *parent)
	: Qt3DRender::QGeometryRenderer(parent), mGeometryFactory{geometryFactory}
{
	log_trace_enter();

	//setVertexCount(-1);

	setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
	//setVertexCount(mGeometry->getNumVerticies());
	setGeometryFactory(mGeometryFactory);

	log_trace_exit();
}

void QuadTreeNodeGeometryRenderer::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
	log_debug("vtx count: %s", vertexCount());
	QGeometryRenderer::sceneChangeEvent(e);
	log_debug("event %s", e);
	log_debug("vtx count: %s", vertexCount());

	if (e->type() == Qt3DCore::PropertyUpdated)
	{
			const auto change = qSharedPointerCast<Qt3DCore::QPropertyUpdatedChange>(e);
			if (change->propertyName() == QLatin1Literal("geometry"))
			{
				//QVariant var = change->value();
				QuadTreeNodeGeometry *geom = dynamic_cast<QuadTreeNodeGeometry*>(geometry());
				//qDebug() << " got geom: " << geom;
				// geom->getNumVerticies()
				log_debug("vtx count: %s", geom->getNumVerticies());
				setVertexCount(geom->getNumVerticies());
				//setPropertyTracking("gwo");
				setGeometry(geom);
 			}

			//QVariant variant = change->value();
			//QString string = variant.toString();

			//ss << " asdf";
	}


}
