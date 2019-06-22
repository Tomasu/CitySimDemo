//
// Created by moose on 05/04/19.
//

#ifndef CITYSIM_QUADTREENODEGEOMETRYGENERATOR_H
#define CITYSIM_QUADTREENODEGEOMETRYGENERATOR_H


#include <Qt3DRender/QBufferDataGenerator>

class QuadTreeNodeGeometry;

class QuadTreeNodeGeometryGenerator : public Qt3DRender::QBufferDataGenerator
{
	public:
		explicit QuadTreeNodeGeometryGenerator(QuadTreeNodeGeometry *geometry)
			: mGeometry(geometry)
		{

		}

		~QuadTreeNodeGeometryGenerator() override
		{
			// nada
		}

		virtual bool operator==(const Qt3DRender::QBufferDataGenerator &other) const;

		QByteArray operator()() final;

		QT3D_FUNCTOR(QuadTreeNodeGeometryGenerator)

	private:
		QuadTreeNodeGeometry *mGeometry;
};


#endif //CITYSIM_QUADTREENODEGEOMETRYGENERATOR_H
