//
// Created by moose on 27/03/19.
//

#ifndef CITYSIM_MAINWINDOW_H
#define CITYSIM_MAINWINDOW_H


#include <Qt3DCore/QAspectEngine>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DInput/QInputAspect>
#include <Qt3DLogic/QLogicAspect>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DRender/QCamera>
#include <Qt3DInput/QInputSettings>
#include <QtGui/QWindow>
#include <Qt3DExtras/QAbstractCameraController>
#include <src/graph/TransportGraph.h>
#include <gdal_priv.h>
#include <src/quadtree/QuadTreeNode.h>
#include <src/aspect/FpsMonitorAspect.h>
#include "RoadData.h"

class MainWindow : public QWindow
{
	public:
		MainWindow(const QString &path);
		~MainWindow();

		Qt3DCore::QEntity *rootEntity();

	private:

		TransportGraph *mGraph;
		QuadTreeNode *mRoadsQuadTree;

		Qt3DCore::QAspectEngine *mAspectEngine;

		// Aspects
		Qt3DRender::QRenderAspect *mRenderAspect;
		Qt3DInput::QInputAspect *mInputAspect;
		Qt3DLogic::QLogicAspect *mLogicAspect;
		FpsMonitorAspect *mFpsAspect;

		// Renderer
		Qt3DRender::QRenderSettings *mRenderSettings;
		Qt3DExtras::QForwardRenderer *mForwardRenderer;
		Qt3DRender::QCamera *mCamera;
		Qt3DExtras::QAbstractCameraController *mCamController;

		// Input
		Qt3DInput::QInputSettings *mInputSettings;

		// Logic

		// Scene
		Qt3DCore::QEntity *mRootEntity;

		// other

		bool mInitialized;

		Qt3DCore::QEntity *createScene(const QString &path);

		Qt3DRender::QCamera *createCamera();
		Qt3DExtras::QAbstractCameraController *createCameraController(Qt3DCore::QEntity *rootEntity, Qt3DRender::QCamera *camera);

		Qt3DCore::QEntity *createRootEntity();

	protected:
		void
		showEvent(QShowEvent *event) override;

		void
		resizeEvent(QResizeEvent *event) override;

		bool
		event(QEvent *event) override;

		void buildRoadGraph(GDALDataset *dataset);
};


#endif //CITYSIM_MAINWINDOW_H
