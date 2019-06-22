//
// Created by moose on 02/04/19.
//

#include "TransportGraph.h"
#include <boost/graph/adjacency_list.hpp>

void TransportGraph::doit()
{

	/*
	 * We want to not create graph vertices for non intersecting points in the gdal data.
	 * So I think as we go through the data, we store vertices as we see them IF they have more than two edges.
	 *  two edges means we just have a  line with a superfluous vertex in the middle.
	 *
	 * This may require preprocessing the data to collect all vertices then loop through all
	 *  "features" and detect their vertices and how many edges each vertex has, etc. any vertices that
	 *  have < 3 edges are dropped (but only from the network graph, not from the geometry.)
	 *
	 *
	 */

}

TransportGraph::TransportGraph()
{

}

TransportGraph::~TransportGraph()
{
}

TransportGraph::Vertex TransportGraph::addVertex(const VertexId &vtxId, float x, float y, float z)
{
	VertexProperties props { vtxId, x, y, z };
	Vertex newVtx = boost::add_vertex(props, mGraph);
	return newVtx;
}

TransportGraph::TransportGraph::Vertex TransportGraph::addVertex(const VertexId& vtxId, const QVector3D& vec)
{
	VertexProperties props { vtxId, vec };
	Vertex newVtx = boost::add_vertex(props, mGraph);
	return newVtx;
}


TransportGraph::Edge TransportGraph::addEdge(const EdgeId &edgeId, const Vertex &a, const Vertex &b)
{
	EdgeProperties props{ edgeId, 1 };
	Edge edge;
	bool inserted;

	boost::tie(edge, inserted) = boost::add_edge(a, b, props, mGraph);
	//mGraph[edge] = props;

	return edge;
}
