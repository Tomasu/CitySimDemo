//
// Created by moose on 04/04/19.
//

#ifndef CITYSIM_ROADDATA_H
#define CITYSIM_ROADDATA_H

#include <string>
#include <vector>

#include "quadtree/QuadTreeNodePointItem.h"
#include "graph/TransportGraph.h"

#include <QVector>
#include <QColor>
#include <QVector3D>

class GeometryBuilder;

class RoadData : public QuadTreeNodePointItem
{
	public:
		explicit RoadData() = delete;
		explicit RoadData(const Rect &bounds, const std::string &mName, const std::string &mType);
		RoadData(const Rect& bounds, const std::string& name, const std::string& type, const std::vector< Point >& points);
		~RoadData() = default;

		RoadData(const RoadData&) = delete;
		RoadData& operator=(const RoadData&) = delete;

		void addFwdEdge(const TransportGraph::Edge &edge, const Point &begin, const Point &end);
		void addRevEdge(const TransportGraph::Edge &edge, const Point &begin, const Point &end);

		std::string getType() { return mType; }

		uint32_t numFwdEdges() { return mFwdEdges.size(); }
		uint32_t numRevEdges() { return mRevEdges.size(); }

		const std::vector<TransportGraph::Edge> &getFwdEdges() { return mFwdEdges; }
		const std::vector<TransportGraph::Edge> &getRevEdges() { return mRevEdges; }

		std::string toString() override;

	private:
		std::string mType;

		std::vector<TransportGraph::Edge> mFwdEdges;
		std::vector<TransportGraph::Edge> mRevEdges;

};


#endif //CITYSIM_ROADDATA_H
