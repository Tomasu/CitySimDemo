//
// Created by moose on 02/04/19.
//

#ifndef CITYSIM_TRANSPORTGRAPH_H
#define CITYSIM_TRANSPORTGRAPH_H

#include <boost/graph/adjacency_list.hpp>
#include "VertexProperties.h"
#include "EdgeProperties.h"

typedef boost::adjacency_list<boost::vecS, boost::listS, boost::directedS, VertexProperties, EdgeProperties, boost::no_property> GraphType;

class TransportGraph
{
	public:
		enum LineType
		{
			// maybe rejig later to also do things like rail/lrt/etc
			// right now it just handles roads.
			LINE_NORMAL = 0,
			LINE_TRUCK,
			LINE_PRIVATE
		};

		enum LineZone
		{
			ZONE_NONE = 0,
			ZONE_RESIDENTIAL,
			ZONE_COMMERCIAL,
			ZONE_INDUSTRIAL
		};

		/*
		 * possible properties lines want:
		 *
		 * Type: normal, truck-ok, private
		 * Capacity: low, med, high, etc?
		 * Zone: Residential, Commercial, Industrial
		 * (no direction, that's implied via the actual edge setup)
		 *
		 */
		/*
		 * Current plan of ignoring non intersecting edges may lead to
		 * too little granularity when calculating weights.
		 * TODO: consider doing some processing to have a max distance between
		 *  verticies and handling all of them in the graph.
		 */
	public:
		typedef boost::graph_traits<GraphType>::edge_descriptor Edge;
		typedef boost::graph_traits<GraphType>::edge_iterator EdgeIterator;

		typedef boost::graph_traits<GraphType>::vertex_descriptor Vertex;
		typedef boost::graph_traits<GraphType>::vertex_iterator VertexIterator;

		TransportGraph();
		virtual ~TransportGraph();

		Vertex addVertex(const VertexId &vtxId, double x, double y);
		Edge addEdge(const EdgeId &edgeId, const Vertex &a, const Vertex &b);

		void doit();

	private:
		GraphType mGraph;
};


#endif //CITYSIM_TRANSPORTGRAPH_H
