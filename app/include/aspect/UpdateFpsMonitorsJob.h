//
// Created by moose on 05/04/19.
//

#ifndef CITYSIM_UPDATEFPSMONITORSJOB_H
#define CITYSIM_UPDATEFPSMONITORSJOB_H


#include <Qt3DCore/QAspectJob>
#include <QSharedPointer>

class FpsMonitorAspect;

class UpdateFpsMonitorsJob : public Qt3DCore::QAspectJob
{
	public:
		explicit UpdateFpsMonitorsJob(FpsMonitorAspect *aspect);

		void setFrameTimeDelta(qint64 dt);
		void run() override;

	private:
		FpsMonitorAspect *m_aspect;
		qint64 m_dt;
};

using UpdateFpsMonitorsJobPtr = QSharedPointer<UpdateFpsMonitorsJob>;


#endif //CITYSIM_UPDATEFPSMONITORSJOB_H
