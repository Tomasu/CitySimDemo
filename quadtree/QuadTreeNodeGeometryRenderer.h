//
// Created by moose on 05/04/19.
//

#ifndef CITYSIM_QUADTREENODEGEOMETRYRENDERER_H
#define CITYSIM_QUADTREENODEGEOMETRYRENDERER_H


#include <Qt3DRender/QGeometryRenderer>

class QuadTreeNodeGeometryFactory;

class QuadTreeNodeGeometryRenderer : public Qt3DRender::QGeometryRenderer
{
	Q_OBJECT
	public:
		explicit QuadTreeNodeGeometryRenderer(QuadTreeNodeGeometryFactory *geometryFactory, Qt3DCore::QNode *parent = nullptr);
		~QuadTreeNodeGeometryRenderer() = default;

		//QSharedPointer<QuadTreeNodeGeometryFactory> geometry() { return mGeometryFactory; }

	protected:
		void
		sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override;

	private:
		QSharedPointer<QuadTreeNodeGeometryFactory> mGeometryFactory;

};


#endif //CITYSIM_QUADTREENODEGEOMETRYRENDERER_H
