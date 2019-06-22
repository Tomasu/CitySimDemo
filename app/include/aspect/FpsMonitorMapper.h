//
// Created by moose on 05/04/19.
//

#ifndef CITYSIM_FPSMONITORMAPPER_H
#define CITYSIM_FPSMONITORMAPPER_H


#include <Qt3DCore/QBackendNodeMapper>
#include "FpsMonitorBackend.h"
#include "FpsMonitorAspect.h"

class FpsMonitorMapper : public Qt3DCore::QBackendNodeMapper
{
	public:
		explicit FpsMonitorMapper(FpsMonitorAspect *aspect);

		Qt3DCore::QBackendNode *create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const override;
		Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const override;
		void destroy(Qt3DCore::QNodeId id) const override;

	private:
		FpsMonitorAspect *m_aspect;
};


#endif //CITYSIM_FPSMONITORMAPPER_H
