//
// Created by moose on 05/04/19.
//

#include "FpsMonitorAspect.h"
#include "FpsMonitorMapper.h"
#include "UpdateFpsMonitorsJob.h"
#include "FpsMonitorComponent.h"


FpsMonitorAspect::FpsMonitorAspect(QObject *parent)
		: Qt3DCore::QAbstractAspect(parent)
		, m_updateMonitorsJob(new UpdateFpsMonitorsJob(this))
		, m_lastTime{0}
{
	// Register the mapper to handle creation, lookup, and destruction of backend nodes
	auto mapper = QSharedPointer<FpsMonitorMapper>::create(this);
	registerBackendType<FpsMonitorComponent>(mapper);
}

void FpsMonitorAspect::addFpsMonitor(Qt3DCore::QNodeId id, FpsMonitorBackend *fpsMonitor)
{
	m_fpsMonitors.insert(id, fpsMonitor);
}

FpsMonitorBackend *FpsMonitorAspect::fpsMonitor(Qt3DCore::QNodeId id)
{
	return m_fpsMonitors.value(id, nullptr);
}

FpsMonitorBackend *FpsMonitorAspect::takeFpsMonitor(Qt3DCore::QNodeId id)
{
	return m_fpsMonitors.take(id);
}

QVector<Qt3DCore::QAspectJobPtr> FpsMonitorAspect::jobsToExecute(qint64 time)
{
	const auto dt = time - m_lastTime;
	m_lastTime = time;
	m_updateMonitorsJob->setFrameTimeDelta(dt);
	return { m_updateMonitorsJob };
}
