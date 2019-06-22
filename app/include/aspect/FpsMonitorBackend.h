//
// Created by moose on 05/04/19.
//

#ifndef CITYSIM_FPSMONITORBACKEND_H
#define CITYSIM_FPSMONITORBACKEND_H


#include <Qt3DCore/QBackendNode>
#include "MovingAverage.h"

class FpsMonitorBackend : public Qt3DCore::QBackendNode
{
	public:
		FpsMonitorBackend();

		void addFpsSample(float fpsSample);

	protected:
		void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e) override;

	private:
		void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change) override;

		int m_rollingMeanFrameCount;
		MovingAverage m_average;
};

#endif //CITYSIM_FPSMONITORBACKEND_H
