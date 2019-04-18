//
// Created by moose on 03/04/19.
//

#ifndef CITYSIM_VERTEXPROPERTIES_H
#define CITYSIM_VERTEXPROPERTIES_H

#include <cstdint>

typedef uint32_t VertexId;

class VertexProperties
{
	public:
		VertexProperties(VertexId id, double x, double y) : mId(id), mX(x), mY(y) { }

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

		double
		getX() const
		{
			return mX;
		}

		void
		setX(double x)
		{
			mX = x;
		}

		double
		getY() const
		{
			return mY;
		}

		void
		setY(double y)
		{
			mY = y;
		}

	private:
		VertexId mId;
		double mX, mY;
};

#include <boost/graph/adjacency_list.hpp>

#endif //CITYSIM_VERTEXPROPERTIES_H
