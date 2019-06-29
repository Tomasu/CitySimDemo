#include "ForwardRenderer.h"
#include "ForwardRenderer_p.h"

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QFrustumCulling>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QCullFace>

static void initResources()
{
	#ifdef QT_STATIC
	Q_INIT_RESOURCE(extras);
	#endif
}

using namespace Qt3DRender;

ForwardRendererPrivate::ForwardRendererPrivate()
: QTechniqueFilterPrivate()
, m_surfaceSelector(new QRenderSurfaceSelector)
, m_viewport(new QViewport())
, m_cameraSelector(new QCameraSelector())
, m_clearBuffer(new QClearBuffers())
, m_frustumCulling(new QFrustumCulling())
, m_renderStateSet(new QRenderStateSet())
, m_cullFace(new QCullFace())
, m_depthTest(new QDepthTest())
{
}
void ForwardRendererPrivate::init()
{
	Q_Q(ForwardRenderer);
	initResources();

	m_renderStateSet->setParent(m_frustumCulling);
	m_renderStateSet->addRenderState(m_cullFace);
	m_renderStateSet->addRenderState(m_depthTest);

	m_cullFace->setMode(Qt3DRender::QCullFace::NoCulling);
	m_depthTest->setDepthFunction(Qt3DRender::QDepthTest::Always);

	m_frustumCulling->setParent(m_clearBuffer);
	m_clearBuffer->setParent(m_cameraSelector);
	m_cameraSelector->setParent(m_viewport);
	m_viewport->setParent(m_surfaceSelector);
	m_surfaceSelector->setParent(q);
	m_viewport->setNormalizedRect(QRectF(0.0f, 0.0f, 1.0f, 1.0f));
	m_clearBuffer->setClearColor(Qt::white);

	m_clearBuffer->setBuffers(QClearBuffers::ColorDepthBuffer);
	QFilterKey *forwardRenderingStyle = new QFilterKey(q);
	forwardRenderingStyle->setName(QStringLiteral("renderingStyle"));
	forwardRenderingStyle->setValue(QStringLiteral("forward"));
	q->addMatch(forwardRenderingStyle);
}
/*!
 *    \class Qt3DExtras::ForwardRenderer
 *    \brief The ForwardRenderer provides a default \l{Qt 3D Render Framegraph}{FrameGraph}
 *    implementation of a forward renderer.
 *    \inmodule Qt3DExtras
 *    \since 5.7
 *    \inherits Qt3DRender::QTechniqueFilter
 *    Forward rendering is what OpenGL traditionally uses. It renders directly to the backbuffer
 *    one object at a time shading each one as it goes.
 *    ForwardRenderer is a single leaf \l{Qt 3D Render Framegraph}{FrameGraph} tree which contains
 *    a Qt3DRender::QViewport, a Qt3DRender::QCameraSelector, and a Qt3DRender::QClearBuffers.
 *    The ForwardRenderer has a default requirement filter key whose name is "renderingStyle" and
 *    value "forward".
 *    If you need to filter out your techniques, you should do so based on that filter key.
 *    By default the viewport occupies the whole screen and the clear color is white.
 *    Frustum culling is also enabled.
 */
/*!
 *    \qmltype ForwardRenderer
 *    \brief The ForwardRenderer provides a default \l{Qt 3D Render Framegraph}{FrameGraph}
 *    implementation of a forward renderer.
 *    \since 5.7
 *    \inqmlmodule Qt3D.Extras
 *    \instantiates Qt3DExtras::ForwardRenderer
 *    Forward rendering is what OpenGL traditionally uses. It renders directly to the backbuffer
 *    one object at a time shading each one as it goes.
 *    ForwardRenderer is a single leaf \l{Qt 3D Render Framegraph}{FrameGraph} tree which contains
 *    a Viewport, a CameraSelector, and a ClearBuffers.
 *    The ForwardRenderer has a default requirement filter key whose name is "renderingStyle" and
 *    value "forward".
 *    If you need to filter out your techniques, you should do so based on that filter key.
 *    By default the viewport occupies the whole screen and the clear color is white.
 *    Frustum culling is also enabled.
 */
ForwardRenderer::ForwardRenderer(QNode *parent)
: QTechniqueFilter(*new ForwardRendererPrivate, parent)
{
	Q_D(ForwardRenderer);
	QObject::connect(d->m_clearBuffer, &QClearBuffers::clearColorChanged, this, &ForwardRenderer::clearColorChanged);
	QObject::connect(d->m_viewport, &QViewport::normalizedRectChanged, this, &ForwardRenderer::viewportRectChanged);
	QObject::connect(d->m_cameraSelector, &QCameraSelector::cameraChanged, this, &ForwardRenderer::cameraChanged);
	QObject::connect(d->m_surfaceSelector, &QRenderSurfaceSelector::surfaceChanged, this, &ForwardRenderer::surfaceChanged);
	QObject::connect(d->m_surfaceSelector, &QRenderSurfaceSelector::externalRenderTargetSizeChanged, this, &ForwardRenderer::externalRenderTargetSizeChanged);
	QObject::connect(d->m_frustumCulling, &QFrustumCulling::enabledChanged, this, &ForwardRenderer::frustumCullingEnabledChanged);
	QObject::connect(d->m_viewport, &QViewport::gammaChanged, this, &ForwardRenderer::gammaChanged);
	d->init();
}
ForwardRenderer::~ForwardRenderer()
{
}
void ForwardRenderer::setViewportRect(const QRectF &viewportRect)
{
	Q_D(ForwardRenderer);
	d->m_viewport->setNormalizedRect(viewportRect);
}
void ForwardRenderer::setClearColor(const QColor &clearColor)
{
	Q_D(ForwardRenderer);
	d->m_clearBuffer->setClearColor(clearColor);
}
void ForwardRenderer::setCamera(Qt3DCore::QEntity *camera)
{
	Q_D(ForwardRenderer);
	d->m_cameraSelector->setCamera(camera);
}
void ForwardRenderer::setSurface(QObject *surface)
{
	Q_D(ForwardRenderer);
	d->m_surfaceSelector->setSurface(surface);
}
void ForwardRenderer::setExternalRenderTargetSize(const QSize &size)
{
	Q_D(ForwardRenderer);
	d->m_surfaceSelector->setExternalRenderTargetSize(size);
}
void ForwardRenderer::setFrustumCullingEnabled(bool enabled)
{
	Q_D(ForwardRenderer);
	d->m_frustumCulling->setEnabled(enabled);
}
void ForwardRenderer::setGamma(float gamma)
{
	Q_D(ForwardRenderer);
	d->m_viewport->setGamma(gamma);
}
/*!
 *    \qmlproperty rect ForwardRenderer::viewportRect
 *    Holds the current normalized viewport rectangle.
 */
/*!
 *    \property ForwardRenderer::viewportRect
 *    Holds the current normalized viewport rectangle.
 */
QRectF ForwardRenderer::viewportRect() const
{
	Q_D(const ForwardRenderer);
	return d->m_viewport->normalizedRect();
}
/*!
 *    \qmlproperty color ForwardRenderer::clearColor
 *    Holds the current clear color of the scene. The frame buffer is initialized to the clear color
 *    before rendering.
 */
/*!
 *    \property ForwardRenderer::clearColor
 *    Holds the current clear color of the scene. The frame buffer is initialized to the clear color
 *    before rendering.
 */
QColor ForwardRenderer::clearColor() const
{
	Q_D(const ForwardRenderer);
	return d->m_clearBuffer->clearColor();
}
/*!
 *    \qmlproperty Entity ForwardRenderer::camera
 *    Holds the current camera entity used to render the scene.
 *    \note A camera is an Entity that has a CameraLens as one of its components.
 */
/*!
 *    \property ForwardRenderer::camera
 *    Holds the current camera entity used to render the scene.
 *    \note A camera is a QEntity that has a QCameraLens as one of its components.
 */
Qt3DCore::QEntity *ForwardRenderer::camera() const
{
	Q_D(const ForwardRenderer);
	return d->m_cameraSelector->camera();
}
/*!
 *    \qmlproperty Object ForwardRenderer::window
 *    Holds the current render surface.
 *    \deprecated
 */
/*!
 *    \property ForwardRenderer::window
 *    Holds the current render surface.
 *    \deprecated
 */
/*!
 *    \qmlproperty Object ForwardRenderer::surface
 *    Holds the current render surface.
 */
/*!
 *    \property ForwardRenderer::surface
 *    Holds the current render surface.
 */
QObject *ForwardRenderer::surface() const
{
	Q_D(const ForwardRenderer);
	return d->m_surfaceSelector->surface();
}
/*!
 *    \property ForwardRenderer::externalRenderTargetSize
 *    Contains the size of the external render target. External render
 *    targets are relevant when rendering does not target a window
 *    surface (as set in \l {ForwardRenderer::surface()}{surface()}).
 */
QSize ForwardRenderer::externalRenderTargetSize() const
{
	Q_D(const ForwardRenderer);
	return d->m_surfaceSelector->externalRenderTargetSize();
}
/*!
 *    \qmlproperty bool ForwardRenderer::frustumCulling
 *    Indicates if the renderer applies frustum culling to the scene.
 */
/*!
 *    \property ForwardRenderer::frustumCulling
 *    Indicates if the renderer applies frustum culling to the scene.
 */
bool ForwardRenderer::isFrustumCullingEnabled() const
{
	Q_D(const ForwardRenderer);
	return d->m_frustumCulling->isEnabled();
}
/*!
 *    \qmlproperty real ForwardRenderer::gamma
 *    Holds the gamma value the renderer applies to the scene.
 */
/*!
 *    \property ForwardRenderer::gamma
 *    Holds the gamma value the renderer applies to the scene.
 */
float ForwardRenderer::gamma() const
{
	Q_D(const ForwardRenderer);
	return d->m_viewport->gamma();
}
