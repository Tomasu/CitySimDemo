//
// Created by moose on 04/04/19.
//

#ifndef CITYSIM_ROADDATA_H
#define CITYSIM_ROADDATA_H

#include <string>
#include <vector>

#include "quadtree/QuadTreeNodeItem.h"
#include "graph/TransportGraph.h"

#include <QVector>
#include <QColor>
#include <QVector3D>

class GeometryBuilder;

class RoadData : public QuadTreeNodeItem
{
	public:
		explicit RoadData() = delete;
		explicit RoadData(const Rect &bounds, const std::string &mName, const std::string &mType);
		RoadData(const Rect& bounds, const std::string& name, const std::string& type, const std::vector< QVector3D >& points);
		~RoadData() = default;

		RoadData(const RoadData&) = delete;
		RoadData& operator=(const RoadData&) = delete;

		void addFwdEdge(const TransportGraph::Edge &edge, const QVector3D &begin, const QVector3D &end);
		void addRevEdge(const TransportGraph::Edge &edge, const QVector3D &begin, const QVector3D &end);

		void addSegment(const QVector3D &start, const QVector3D &end);

		std::string getName() { return mName; }
		std::string getType() { return mType; }

		uint32_t numFwdEdges() { return mFwdEdges.size(); }
		uint32_t numRevEdges() { return mRevEdges.size(); }

		const std::vector<TransportGraph::Edge> &getFwdEdges() { return mFwdEdges; }
		const std::vector<TransportGraph::Edge> &getRevEdges() { return mRevEdges; }

		const std::vector<QVector3D> &getPoints() const { return mPoints; }

		std::vector<QuadTreeNodeItem*> split(const Rect &bounds, const Point &crossPoint) override;

		bool isPartiallyContainedByRect(const Rect & rect) const override;

		bool isContainedByRect(const Rect & rect) const override;

		std::string toString() override;

	private:
		std::string mName;
		std::string mType;

		std::vector<QVector3D> mPoints;

		std::vector<TransportGraph::Edge> mFwdEdges;
		std::vector<TransportGraph::Edge> mRevEdges;

		void buildGeometry(GeometryBuilder *gb) override;
};


#endif //CITYSIM_ROADDATA_H
