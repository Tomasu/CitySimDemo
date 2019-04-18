//
// Created by moose on 05/04/19.
//

#include "UpdateFpsMonitorsJob.h"

#include "FpsMonitorAspect.h"
#include "FpsMonitorBackend.h"

UpdateFpsMonitorsJob::UpdateFpsMonitorsJob(FpsMonitorAspect *aspect)
		: Qt3DCore::QAspectJob()
		, m_aspect(aspect)
{
	Q_ASSERT(aspect);
}

void UpdateFpsMonitorsJob::setFrameTimeDelta(qint64 dt)
{
	m_dt = dt;
}

void UpdateFpsMonitorsJob::run()
{
	// Iterate over all the enabled fps monitors and add
	// the instantaneous fps value to their moving averages.
	const float fps = 1.0f / (float(m_dt) * 1.0e-9);
	const auto &fpsMonitors = m_aspect->fpsMonitors();
	for (const auto fpsMonitor : fpsMonitors) {
		// Don't process disabled monitors
		if (!fpsMonitor->isEnabled())
			continue;

		fpsMonitor->addFpsSample(fps);
	}
}
