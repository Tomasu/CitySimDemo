#ifndef CITYSIM_MAINWINDOW_H
#define CITYSIM_MAINWINDOW_H

//
// Created by moose on 27/03/19.
//


#include <QtGui/QWindow>
#include <QPoint>

#include <gdal_priv.h>

class FpsMonitorAspect;
class QuadTree;
class TransportGraph;
class QuadTreeNodeMapper;
class QuadTreeNodeEntityFactory;
class ForwardRenderer;

namespace Qt3DInput {
	class QMouseDevice;
	class QMouseHandler;
	class QMouseEvent;
	class QInputAspect;
	class QInputSettings;
}

namespace Qt3DCore {
	class QEntity;
	class QAspectEngine;
	class QTransform;
}

namespace Qt3DRender {
	class QCamera;
	class QRenderSettings;
	class QRenderAspect;
	class QScreenRayCaster;
}

namespace Qt3DExtras {
	class QForwardRenderer;
	class QAbstractCameraController;
	class QPlaneMesh;
}

namespace Qt3DLogic {
	class QLogicAspect;
	class QFrameAction;
}

class MainWindow : public QWindow
{
	public:
		MainWindow(const QString &path);
		~MainWindow();

		Qt3DCore::QEntity *rootEntity();
		Qt3DCore::QEntity *mapEntity();

		Qt3DRender::QCamera *camera() const { return mCamera; }

		Qt3DInput::QMouseHandler *mouseHandler() const { return mMouseHandler; }

	private:

		TransportGraph *mGraph;
		QuadTree *mRoadsQuadTree;

		Qt3DCore::QAspectEngine *mAspectEngine;

		// Aspects
		Qt3DRender::QRenderAspect *mRenderAspect;
		Qt3DInput::QInputAspect *mInputAspect;
		Qt3DLogic::QLogicAspect *mLogicAspect;
		FpsMonitorAspect *mFpsAspect;

		// Renderer
		Qt3DRender::QRenderSettings *mRenderSettings;
		ForwardRenderer *mForwardRenderer;
		Qt3DRender::QCamera *mCamera;
		Qt3DExtras::QAbstractCameraController *mCamController;
		Qt3DRender::QScreenRayCaster *mRayCaster;

		// Input
		Qt3DInput::QInputSettings *mInputSettings;

		Qt3DInput::QMouseDevice *mMouseDevice;
		Qt3DInput::QMouseHandler *mMouseHandler;

		// Logic
		Qt3DLogic::QFrameAction *mFrameAction;

		// Scene
		Qt3DCore::QEntity *mRootEntity;
		Qt3DCore::QTransform *mRootTransform;

		Qt3DCore::QEntity *mMapEntity;
		Qt3DExtras::QPlaneMesh *mMapMesh;
		Qt3DCore::QTransform *mMapTransform;

		// other


		bool mInitialized;

		void createScene(Qt3DCore::QEntity *rootEntity, const QString &path);

		Qt3DRender::QCamera *createCamera(Qt3DCore::QEntity *rootEntity);
		Qt3DExtras::QAbstractCameraController *createCameraController(Qt3DCore::QEntity *rootEntity, Qt3DRender::QCamera *camera);

		Qt3DCore::QEntity *createRootEntity();

		QuadTreeNodeMapper *mNodeMapper;
		QuadTreeNodeEntityFactory *mEntityFactory;

		QPoint mMousePos;
		bool mMouseMoved;

	protected:
		void
		showEvent(QShowEvent *event) override;

		void
		resizeEvent(QResizeEvent *event) override;

		bool
		event(QEvent *event) override;

		void buildRoadGraph(Qt3DCore::QEntity *parentEntity, GDALDataset *dataset);

		void mousePositionChanged(Qt3DInput::QMouseEvent *ev);
		void frameActionTriggered(float dt);
};


#endif //CITYSIM_MAINWINDOW_H
