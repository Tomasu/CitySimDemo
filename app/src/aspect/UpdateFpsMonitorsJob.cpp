//
// Created by moose on 05/04/19.
//

#include "aspect/UpdateFpsMonitorsJob.h"

#include "aspect/FpsMonitorAspect.h"
#include "aspect/FpsMonitorBackend.h"

UpdateFpsMonitorsJob::UpdateFpsMonitorsJob(FpsMonitorAspect *aspect)
		: Qt3DCore::QAspectJob(),
		m_aspect(aspect),
		m_dt{0}
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
