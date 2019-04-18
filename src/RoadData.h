//
// Created by moose on 04/04/19.
//

#ifndef CITYSIM_ROADDATA_H
#define CITYSIM_ROADDATA_H

#include <string>
#include <vector>
#include "quadtree/QuadTreeNodeItem.h"
#include "graph/TransportGraph.h"
#include "GeometryBuilder.h"

class RoadData : public QuadTreeNodeItem
{
	public:
		explicit RoadData() = delete;
		explicit RoadData(const QRectF &bounds, const std::string &mName, const std::string &mType);
		~RoadData() = default;

		RoadData(const RoadData&) = delete;
		RoadData& operator=(const RoadData&) = delete;

		void addFwdEdge(TransportGraph::Edge edge, const OGRPoint &begin, const OGRPoint &end);
		void addRevEdge(TransportGraph::Edge edge, const OGRPoint &begin, const OGRPoint &end);

		std::string getName() { return mName; }
		std::string getType() { return mType; }

		uint32_t numFwdEdges() { return mFwdEdges.size(); }
		uint32_t numRevEdges() { return mRevEdges.size(); }

		const std::vector<TransportGraph::Edge> &getFwdEdges() { return mFwdEdges; }
		const std::vector<TransportGraph::Edge> &getRevEdges() { return mRevEdges; }

	private:
		std::string mName;
		std::string mType;
		QVector<QPointF> mPoints;

		std::vector<TransportGraph::Edge> mFwdEdges;
		std::vector<TransportGraph::Edge> mRevEdges;

		void buildGeometry(GeometryBuilder *gb) override;
};

#endif //CITYSIM_ROADDATA_H
