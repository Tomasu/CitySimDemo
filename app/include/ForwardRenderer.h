#ifndef CITYSIM_FORWARDRENDERER_H
#define CITYSIM_FORWARDRENDERER_H

#include <Qt3DRender/QTechniqueFilter>
#include <QtGlobal>
#include <QRectF>
#include <QColor>

class ForwardRendererPrivate;
class ForwardRenderer : public Qt3DRender::QTechniqueFilter
{
	Q_OBJECT
	Q_PROPERTY(QObject *surface READ surface WRITE setSurface NOTIFY surfaceChanged)
	Q_PROPERTY(QObject *window READ surface WRITE setSurface NOTIFY surfaceChanged)
	Q_PROPERTY(QRectF viewportRect READ viewportRect WRITE setViewportRect NOTIFY viewportRectChanged)
	Q_PROPERTY(QColor clearColor READ clearColor WRITE setClearColor NOTIFY clearColorChanged)
	Q_PROPERTY(Qt3DCore::QEntity *camera READ camera WRITE setCamera NOTIFY cameraChanged)
	Q_PROPERTY(QSize externalRenderTargetSize READ externalRenderTargetSize WRITE setExternalRenderTargetSize NOTIFY externalRenderTargetSizeChanged)
	Q_PROPERTY(bool frustumCulling READ isFrustumCullingEnabled WRITE setFrustumCullingEnabled NOTIFY frustumCullingEnabledChanged)
	Q_PROPERTY(float gamma READ gamma WRITE setGamma NOTIFY gammaChanged REVISION 9)
	public:
		explicit ForwardRenderer(Qt3DCore::QNode *parent = nullptr);
		~ForwardRenderer();
		QRectF viewportRect() const;
		QColor clearColor() const;
		Qt3DCore::QEntity *camera() const;
		QObject *surface() const;
		QSize externalRenderTargetSize() const;
		bool isFrustumCullingEnabled() const;
		float gamma() const;
	public Q_SLOTS:
		void setViewportRect(const QRectF &viewportRect);
		void setClearColor(const QColor &clearColor);
		void setCamera(Qt3DCore::QEntity *camera);
		void setSurface(QObject * surface);
		void setExternalRenderTargetSize(const QSize &size);
		void setFrustumCullingEnabled(bool enabled);
		void setGamma(float gamma);
	Q_SIGNALS:
		void viewportRectChanged(const QRectF &viewportRect);
		void clearColorChanged(const QColor &clearColor);
		void cameraChanged(Qt3DCore::QEntity *camera);
		void surfaceChanged(QObject *surface);
		void externalRenderTargetSizeChanged(const QSize &size);
		void frustumCullingEnabledChanged(bool enabled);
		void gammaChanged(float gamma);
	private:
		Q_DECLARE_PRIVATE(ForwardRenderer)
};

#endif // CITYSIM_FORWARDRENDERER_H
