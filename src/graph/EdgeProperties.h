//
// Created by moose on 03/04/19.
//

#ifndef CITYSIM_EDGEPROPERTIES_H
#define CITYSIM_EDGEPROPERTIES_H

#include <cstdint>

typedef uint32_t EdgeId;

class EdgeProperties
{
	public:
		EdgeProperties(EdgeId id, int weight) : mId(id), mWeight(weight) {}
		EdgeId getId() const;

		void
		setId(EdgeId id);

		int
		getWeight() const;

		void
		setWeight(int weight);

	private:
		EdgeId mId;
		int mWeight;
};

#include <boost/graph/adjacency_list.hpp>

#endif //CITYSIM_EDGEPROPERTIES_H
