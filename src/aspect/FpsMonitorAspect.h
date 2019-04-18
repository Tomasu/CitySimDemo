//
// Created by moose on 05/04/19.
//

#ifndef CITYSIM_FPSMONITORASPECT_H
#define CITYSIM_FPSMONITORASPECT_H


#include <Qt3DCore/QAbstractAspect>
#include "FpsMonitorBackend.h"
#include "UpdateFpsMonitorsJob.h"

class FpsMonitorBackend;

class FpsMonitorAspect : public Qt3DCore::QAbstractAspect
{
	Q_OBJECT
	public:
		explicit FpsMonitorAspect(QObject *parent = nullptr);

		void addFpsMonitor(Qt3DCore::QNodeId id, FpsMonitorBackend *fpsMonitor);
		FpsMonitorBackend *fpsMonitor(Qt3DCore::QNodeId id);
		FpsMonitorBackend *takeFpsMonitor(Qt3DCore::QNodeId id);

		const QHash<Qt3DCore::QNodeId, FpsMonitorBackend *> &fpsMonitors() const
		{
			return m_fpsMonitors;
		}

	protected:
		QVector<Qt3DCore::QAspectJobPtr> jobsToExecute(qint64 time) override;

	private:
		QHash<Qt3DCore::QNodeId, FpsMonitorBackend *> m_fpsMonitors;
		UpdateFpsMonitorsJobPtr m_updateMonitorsJob;
		qint64 m_lastTime;
};


#endif //CITYSIM_FPSMONITORASPECT_H
