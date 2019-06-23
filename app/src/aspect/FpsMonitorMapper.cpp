//
// Created by moose on 05/04/19.
//

#include "aspect/FpsMonitorMapper.h"


FpsMonitorMapper::FpsMonitorMapper(FpsMonitorAspect *aspect)
		: m_aspect(aspect)
{
	Q_ASSERT(m_aspect);
}

Qt3DCore::QBackendNode *FpsMonitorMapper::create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const
{
	auto fpsMonitor = new FpsMonitorBackend;
	m_aspect->addFpsMonitor(change->subjectId(), fpsMonitor);
	return fpsMonitor;
}

Qt3DCore::QBackendNode *FpsMonitorMapper::get(Qt3DCore::QNodeId id) const
{
	return m_aspect->fpsMonitor(id);
}

void FpsMonitorMapper::destroy(Qt3DCore::QNodeId id) const
{
	auto fpsMonitor = m_aspect->takeFpsMonitor(id);
	delete fpsMonitor;
}
