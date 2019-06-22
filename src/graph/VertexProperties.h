//
// Created by moose on 03/04/19.
//

#ifndef CITYSIM_VERTEXPROPERTIES_H
#define CITYSIM_VERTEXPROPERTIES_H

#include <cstdint>

#include <QVector3D>

typedef uint32_t VertexId;

class VertexProperties
{
	public:
		VertexProperties(VertexId id, float x, float y, float z) : mId(id), mX(x), mY(y), mZ(z) { }
		VertexProperties(VertexId id, const QVector3D &vec) : mId(id), mX(vec.x()), mY(vec.y()), mZ(vec.z()) { }

		VertexId
		getId() const
		{
			return mId;
		}

		void
		setId(VertexId id)
		{
			mId = id;
		}

		float
		getX() const
		{
			return mX;
		}

		void
		setX(float x)
		{
			mX = x;
		}

		float
		getY() const
		{
			return mY;
		}

		void
		setY(float y)
		{
			mY = y;
		}

		float
		getZ() const
		{
			return mZ;
		}

		void
		setZ(float z)
		{
			mZ = z;
		}

	private:
		VertexId mId;
		float mX, mY, mZ;
};

#include <boost/graph/adjacency_list.hpp>

#endif //CITYSIM_VERTEXPROPERTIES_H
