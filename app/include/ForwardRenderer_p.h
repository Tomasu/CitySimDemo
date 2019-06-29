#ifndef CITYSIM_FORWARDRENDERER_P_H_GUARD
#define CITYSIM_FORWARDRENDERER_P_H_GUARD

#include <QtGlobal>
#include <Qt3DRender/private/qtechniquefilter_p.h>

namespace Qt3DRender {
	class QViewport;
	class QClearBuffers;
	class QCameraSelector;
	class QFrustumCulling;
	class QRenderSurfaceSelector;
	class QRenderStateSet;
	class QCullFace;
	class QDepthTest;
} // namespace Qt3DRender

class ForwardRenderer;

class ForwardRendererPrivate: public Qt3DRender::QTechniqueFilterPrivate
{
	public:
		ForwardRendererPrivate();
		Qt3DRender::QRenderSurfaceSelector *m_surfaceSelector;
		Qt3DRender::QViewport *m_viewport;
		Qt3DRender::QCameraSelector *m_cameraSelector;
		Qt3DRender::QClearBuffers *m_clearBuffer;
		Qt3DRender::QFrustumCulling *m_frustumCulling;
		Qt3DRender::QRenderStateSet *m_renderStateSet;
		Qt3DRender::QCullFace *m_cullFace;
		Qt3DRender::QDepthTest *m_depthTest;

		void init();
		Q_DECLARE_PUBLIC(ForwardRenderer)
};

#endif /* CITYSIM_FORWARDRENDERER_P_H_GUARD */
