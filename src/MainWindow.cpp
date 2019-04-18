//
// Created by moose on 27/03/19.
//

#include <QtGui/QOpenGLContext>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QPointLight>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QCullFace>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QLineWidth>
#include <Qt3DExtras/QFirstPersonCameraController>
#include "MainWindow.h"
#include "ShpMeshGeometry.h"
#include "ShpEntity.h"
#include <shapefil.h>
#include <Qt3DExtras/QPerVertexColorMaterial>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <src/aspect/FpsMonitorComponent.h>
#include <iostream>

#include "LogUtils.h"

#include "quadtree/QuadTreeNodeEntityFactory.h"
#include "RootEntity.h"

#define TAG "MainWindow"

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DInput;
using namespace Qt3DLogic;
using namespace Qt3DExtras;


class MainQuadTreeNodeEntityFactory : public QuadTreeNodeEntityFactory
{
	public:
		MainQuadTreeNodeEntityFactory(MainWindow *mw, Qt3DCore::QNode *root)
			: QuadTreeNodeEntityFactory(root), mMainWindow(mw)
		{

		}

		QuadTreeNodeEntity *
		operator()(QuadTreeNode *node) override
		{
			log_trace_enter();
			QuadTreeNodeEntity *newEntity = new QuadTreeNodeEntity(node, rootNode());
			newEntity->setParent(rootNode());
			return newEntity;
		}

	private:
		MainWindow *mMainWindow;
};

Qt3DCore::QEntity *loadShp(const QString &path);

MainWindow::MainWindow(const QString &path)
{
	mInitialized = false;

	mGraph = new TransportGraph();

	mRootEntity = createRootEntity();

	mAspectEngine = new QAspectEngine;
	mRenderAspect = new QRenderAspect;
	mInputAspect = new QInputAspect;
	mLogicAspect = new QLogicAspect;
	mRenderSettings = new QRenderSettings;
	//mRenderSettings->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::LinePicking);

	mForwardRenderer = new QForwardRenderer;

	mForwardRenderer->setEnabled(true);
	mForwardRenderer->setFrustumCullingEnabled(false);

	mCamera = createCamera();
	mCamera->setParent(mRootEntity);
	mCamController = createCameraController(mRootEntity, mCamera);

	mInputSettings = new QInputSettings;

	mForwardRenderer->setClearColor(QColor(QRgb(0xffffff)));

	mFpsAspect = new FpsMonitorAspect;

	setSurfaceType(QSurface::OpenGLSurface);

	resize(1024, 768);

	QSurfaceFormat format = QSurfaceFormat::defaultFormat();
	if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
		format.setVersion(4, 5);
		format.setProfile(QSurfaceFormat::CoreProfile);
	}

	format.setDepthBufferSize(24);
	format.setSamples(4);
	format.setStencilBufferSize(8);

	setFormat(format);
	QSurfaceFormat::setDefaultFormat(format);

	mAspectEngine->registerAspect(mRenderAspect);
	mAspectEngine->registerAspect(mInputAspect);
	mAspectEngine->registerAspect(mLogicAspect);
	mAspectEngine->registerAspect(mFpsAspect);

//	QLineWidth *lineWidth = new QLineWidth(mRenderSettings);
//	lineWidth->setValue(15.0f);

	mCamera->setParent(mRootEntity);
	mForwardRenderer->setCamera(mCamera);
	mForwardRenderer->setSurface(this);
	mRenderSettings->setActiveFrameGraph(mForwardRenderer);

	mInputSettings->setEventSource(this);

	mRootEntity = createScene(path);
}

MainWindow::~MainWindow()
{
	//delete mRenderAspect;
	//delete mInputAspect;
	//delete mLogicAspect;
	delete mAspectEngine;
	//delete mRenderSettings;
	//delete mForwardRenderer;
	//delete mCamera;
	//delete mInputSettings;
	//delete mRootEntity;
}

//void recurseDirLoad(const QFileInfo info, QEntity *rootEntity);

QString shpPath(const QFileInfo &fi);

GDALDataset *openDataset(const QString path)
{
	QString realPath = path + ".shp";
	GDALDataset *dataset = static_cast<GDALDataset *>(GDALOpenEx(realPath.toLatin1().data(), GDAL_OF_READONLY | GDAL_OF_VECTOR | GDAL_OF_VERBOSE_ERROR, NULL, NULL, NULL));

	if (dataset == nullptr)
	{
		log_error("error opening with gdal?!");
		return nullptr;
	}

	return dataset;
}

#define CLASS_CD_AA "A-A"
#define CLASS_CD_AB "A-B"
#define CLASS_CD_AC "A-C"
#define CLASS_CD_AD "A-D"
#define CLASS_CD_ALR "AL-R"
#define CLASS_CD_CC "C-C"
#define CLASS_CD_CI "C-I"
#define CLASS_CD_CR "C-R"
#define CLASS_CD_LC "L-C"
#define CLASS_CD_LI "L-I"
#define CLASS_CD_LP "L-P"
#define CLASS_CD_LPW "L-PW"
#define CLASS_CD_LR "L-R"

#define DIGITZ_1F_ONEWAY "1F"
#define DIGITZ_1R_ONEWAY_REV "1R"
#define DIGITZ_2B_TWOWAY "2B"
#define DIGITZ_2F_TRUE_ONEWAY "2F"
#define DIGITZ_2R_TRUE_ONEWAY_REV "2R"

bool hasForwardEdge(const char *str)
{
	return strcmp(str, DIGITZ_2B_TWOWAY) == 0
		|| strcmp(str, DIGITZ_1F_ONEWAY) == 0
		|| strcmp(str, DIGITZ_2F_TRUE_ONEWAY) == 0;
}

bool hasReverseEdge(const char *str)
{
	return strcmp(str, DIGITZ_2B_TWOWAY) == 0
		|| strcmp(str, DIGITZ_1R_ONEWAY_REV) == 0
		|| strcmp(str, DIGITZ_2R_TRUE_ONEWAY_REV) == 0;
}

void MainWindow::buildRoadGraph(GDALDataset *dataset)
{
	log_trace_enter();

	OGRLayer *layer = dataset->GetLayer(0);

	layer->ResetReading();

	OGRFeature *poFeature = nullptr;

	OGRSpatialReference *layerSpatialRef = layer->GetSpatialRef();

	OGRSpatialReference mercSrs;
	OGRErr retr = mercSrs.importFromEPSG(3776);
	if (retr != OGRERR_NONE)
	{
		log_error("couldnt get well known CS for transform... ");
		return;
	}

	OGRCoordinateTransformation *coordTransform = OGRCreateCoordinateTransformation(layerSpatialRef, &mercSrs);

	OGREnvelope envelope;
	OGRErr extret = layer->GetExtent(&envelope, 1);
	if (extret != OGRERR_NONE)
	{
		log_error("failed to get layer extent :(");
		return;
	}

	double extentX[2] = { envelope.MinX, envelope.MaxX };
	double extentY[2] = { envelope.MinY, envelope.MaxY };
	double extentZ[2] = { 0.0, 0.0 };

	coordTransform->Transform(2, extentX, extentY, extentZ );

	QRectF extRect = QRectF(QPointF(extentX[0], extentY[0]), QPointF(extentX[1], extentY[1]));
	mRoadsQuadTree = new QuadTreeNode(new MainQuadTreeNodeEntityFactory(this, mRootEntity), extRect);

	typedef std::pair<double,double> DoublePair;
	std::map<DoublePair, TransportGraph::Vertex> vertices;

	while ((poFeature = layer->GetNextFeature()))
	{
		// class_cd & dititz
		int classCdIdx = poFeature->GetFieldIndex("class_cd");
		int digitzIdx = poFeature->GetFieldIndex("digitiz_cd");
		int nameAbPrIdx = poFeature->GetFieldIndex("name_ab_pr");

		const char *classCd = poFeature->GetFieldAsString(classCdIdx);
		const char *digitz = poFeature->GetFieldAsString(digitzIdx);
		const char *nameAbPr = poFeature->GetFieldAsString(nameAbPrIdx);

		OGRGeometry *geometry = poFeature->GetGeometryRef();
		if (geometry == nullptr)
		{
			log_error("no geometry?");
			continue;
		}

		geometry->transform(coordTransform);

		OGRwkbGeometryType type = wkbFlatten(geometry->getGeometryType());

		if (type != wkbLineString)
		{
			log_warn("WARNING: unknown feature type: %s", geometry->getGeometryName());
			continue;
		}

		OGRLineString *lineString = geometry->toLineString();
		OGREnvelope geomEnvelope;
		lineString->getEnvelope(&geomEnvelope);

		QRectF geomBounds{
			geomEnvelope.MinX, geomEnvelope.MinY,
			geomEnvelope.MaxX - geomEnvelope.MinX, geomEnvelope.MaxY - geomEnvelope.MinY
		};

		RoadData *itemData = new RoadData(geomBounds, nameAbPr, classCd);

		mRoadsQuadTree->insertItem(itemData);

		TransportGraph::Vertex vtx1;

		int numPoints = lineString->getNumPoints();
		for (int i = 0; i < numPoints-1; ++i)
		{
			OGRPoint startPt;
			lineString->getPoint(i, &startPt);

			OGRPoint endPt;
			lineString->getPoint(i+1, &endPt);

			auto vtx1it = vertices.find(std::make_pair(startPt.getX(), startPt.getY()));
			if (vtx1it == vertices.end())
			{
				vtx1 = mGraph->addVertex(0, startPt.getX(), startPt.getY());
			}
			else
			{
				vtx1 = vtx1it->second;
			}

			TransportGraph::Vertex vtx2;
			auto vtx2it = vertices.find(std::make_pair(endPt.getX(), endPt.getY()));
			if (vtx2it == vertices.end())
			{
				vtx2 = mGraph->addVertex(0, endPt.getX(), endPt.getY());
			}
			else
			{
				vtx2 = vtx2it->second;
			}

			bool makeForwardEdge = hasForwardEdge(digitz);
			bool makeReverseEdge = hasReverseEdge(digitz);

			if (makeForwardEdge)
			{
				TransportGraph::Edge forwardEdge = mGraph->addEdge(poFeature->GetFID(), vtx1, vtx2);
				itemData->addFwdEdge(forwardEdge, startPt, endPt);
			}

			if (makeReverseEdge)
			{
				TransportGraph::Edge reverseEdge = mGraph->addEdge(poFeature->GetFID(), vtx2, vtx1);
				itemData->addRevEdge(reverseEdge, startPt, endPt);
			}
		}
	}

	log_trace_exit();
}

QEntity *MainWindow::createScene(const QString &path)
{
//	QCylinderMesh *cylinderMesh = new QCylinderMesh(mRootEntity);
//	cylinderMesh->setRings(50);
//	cylinderMesh->setSlices(30);
//	cylinderMesh->setRadius(2.5f);
//	cylinderMesh->setLength(5.0f);
//
//	// Transform for cylinder
//	Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
//	transform->setScale(1.5f);
//	transform->setTranslation(QVector3D(10.0f, 10.0f, 0.0f));
//	transform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 45.0f));
//
//	// Material
//	QPhongMaterial *material = new QPhongMaterial(mRootEntity);
//	material->setDiffuse(Qt::red);
//
//	// Cylinder
//	Qt3DCore::QEntity *cylinder = new Qt3DCore::QEntity(mRootEntity);
//	cylinder->addComponent(cylinderMesh);
//	cylinder->addComponent(transform);
//	cylinder->addComponent(material);


	QEntity *lightEntity = new QEntity(mRootEntity);
	lightEntity->setObjectName("lightEntity");
	QPointLight *light = new QPointLight(lightEntity);
	light->setColor("white");
	light->setIntensity(1);
	lightEntity->addComponent(light);

	Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(lightEntity);
	lightTransform->setTranslation(mCamera->position());
	lightEntity->addComponent(lightTransform);

	FpsMonitorComponent *fpsMonitor = new FpsMonitorComponent(mRootEntity);
	fpsMonitor->setRollingMeanFrameCount(60);
//	connect(fpsMonitor, &FpsMonitorComponent::framesPerSecondChanged, [&](float rmfc) {
//		std::cout << " fps: " << rmfc << std::endl;
//	});

	mRootEntity->addComponent(fpsMonitor);


	if (!path.isEmpty())
	{
		QFileInfo fileInfo(path);
		if (fileInfo.isDir())
		{
			log_debug("got dir: %s", qPrintable(path));

			QDir dir(path);

			//SHPEntity *roadEntity = new SHPEntity;

			QString roadsShpPath = shpPath(QFileInfo(dir, "roads"));
			GDALDataset *roadsDataset = openDataset(roadsShpPath);
// 			if(!roadEntity->load(roadsDataset))
// 			{
// 				qDebug() << " failed to load roads";
// 			}

			buildRoadGraph(roadsDataset);

// 			roadEntity->getTransform()->setScale(0.05f);
//
// 			QVector3D roadOrigin = roadEntity->origin() * 0.05f;
//
// 			roadEntity->setParent(mRootEntity);
//
// 			SHPEntity *rooflineEntity = new SHPEntity;
// 			QString rooflineShpPath = shpPath(QFileInfo(dir, "rooflines"));
// 			GDALDataset *roofDataset = openDataset(rooflineShpPath);
// 			if(!rooflineEntity->load(roofDataset))
// 			{
// 				log_error("failed to load rooflines");
// 			}
//
// 			QVector3D rooflineOrigin = rooflineEntity->origin() * 0.05f;
// 			QVector3D diffOrigin = rooflineOrigin - roadOrigin;
//
// 			log_debug("roofline orig: %s, road orig: %s", rooflineOrigin, roadOrigin);
// 			log_debug("diff orig: %s", diffOrigin);
//
// 			Qt3DCore::QTransform *rooflineTransform = rooflineEntity->getTransform();
// 			rooflineTransform->setTranslation(diffOrigin);
// 			rooflineTransform->setScale(0.05f);
//
// 			rooflineEntity->setParent(mRootEntity);
		}
	}


	return mRootEntity;
}

QString shpPath(const QFileInfo &fi)
{
	if(!fi.isDir())
	{
		log_error("not a dir!");
		return "";
	}

	QDir dir(fi.filePath());

	QFileInfoList fileInfoList = dir.entryInfoList(
			QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);

	for (QFileInfo entInfo: fileInfoList)
	{
		if (entInfo.suffix() == "shp" || entInfo.suffix() == "shx")
		{
			QFileInfo fileInfo(entInfo.dir(), entInfo.baseName());
			log_debug("found: %s", qPrintable(fileInfo.filePath()));
			return fileInfo.filePath();
		}
	}

	return "";
}
//
//void dirLoad(const QString &dirName, QEntity *rootEntity)
//{
//	qDebug() << "dirLoad: " << dirName;
//
//	QEntity *shpEntity = loadShp(dirName);
//	shpEntity->setParent(rootEntity);
//}
//
//void recurseDirLoad(const QFileInfo info, QEntity *rootEntity)
//{
//	qDebug() << "recurseDirLoad: " << info;
//
//	if(!info.isDir())
//	{
//		qDebug() << " not a dir!";
//		return;
//	}
//
//	QDir dir(info.filePath());
//	qDebug() << " dir: " << dir;
//
//	QFileInfoList fileInfoList = dir.entryInfoList(
//			QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
//
//	qDebug() << " infoList: " << fileInfoList;
//
//	for (QFileInfo entInfo: fileInfoList)
//	{
//		qDebug() << " entry: " << entInfo;
//
//		if(entInfo.isDir())
//		{
//			recurseDirLoad(entInfo, rootEntity);
//		}
//		else
//		{
//			//qDebug() << " file: suffix=" << entInfo.suffix();
//			if (entInfo.suffix() == "shp" || entInfo.suffix() == "shx")
//			{
//				QFileInfo fileInfo(entInfo.dir(), entInfo.baseName());
//
//				dirLoad(fileInfo.filePath(), rootEntity);
//				return;
//			}
//		}
//	}
//}

QCamera *MainWindow::createCamera()
{
	// Camera
	QCamera *cameraEntity = new QCamera;
	cameraEntity->setObjectName("camera");
	cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 2000.0f);
	cameraEntity->setPosition(QVector3D(0, 0, 100.0f));
	cameraEntity->setUpVector(QVector3D(0, 1, 0));
	cameraEntity->setViewCenter(QVector3D(0, 0, 0));

	return cameraEntity;
}

QAbstractCameraController *MainWindow::createCameraController(QEntity *rootEntity, QCamera *camera)
{
//	QOrbitCameraController *camController = new QOrbitCameraController(rootEntity);

	auto *camController = new QFirstPersonCameraController(rootEntity);
	camController->setLinearSpeed(90.0f);
	camController->setLookSpeed(180.0f);
	camController->setCamera(camera);

	return camController;
}

Qt3DCore::QEntity *MainWindow::createRootEntity()
{
	QEntity *entity = new RootEntity;
	entity->setObjectName("rootEntity");

	return entity;
}

void MainWindow::showEvent(QShowEvent *event)
{
	if (!mInitialized)
	{
		mRootEntity->addComponent(mRenderSettings);
		mRootEntity->addComponent(mInputSettings);
		mAspectEngine->setRootEntity(Qt3DCore::QEntityPtr(mRootEntity));
		mInitialized = true;
	}

	QWindow::showEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *)
{
	//QWindow::resizeEvent(event);
	mCamera->setAspectRatio(float(width()) / float(height()));
}

bool MainWindow::event(QEvent *event)
{
	const bool needsRedraw = (event->type() == QEvent::Expose || event->type() == QEvent::UpdateRequest);
	if (needsRedraw && mRenderSettings->renderPolicy() == Qt3DRender::QRenderSettings::OnDemand)
	{
		mRenderSettings->sendCommand(QLatin1Literal("InvalidateFrame"));
	}

	return QWindow::event(event);
}

Qt3DCore::QEntity *MainWindow::rootEntity()
{
	return mRootEntity;
}

//
//Qt3DCore::QEntity *loadShp(const QString &path)
//{
//	qDebug() << "loadShp: " << path;
//
//	Qt3DCore::QEntity *customMeshEntity = new Qt3DCore::QEntity();
//	customMeshEntity->setObjectName("customMeshEntity");
//
//	Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
//	//transform->setScale(0.01f); // ??
//	//transform->setRotationY(rot);
//	transform->setTranslation(QVector3D(0, 0, 0.0f));
//
//	QMaterial *material = new QPerVertexColorMaterial(customMeshEntity);
//
//	QGeometryRenderer *customMeshRenderer = new Qt3DRender::QGeometryRenderer();
//
//	SHPMeshGeometry *shpMesh = new SHPMeshGeometry(path, customMeshRenderer);
//
//	customMeshRenderer->setInstanceCount(1);
//	customMeshRenderer->setIndexOffset(0);
//	customMeshRenderer->setFirstInstance(0);
//	customMeshRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
//	//customMeshRenderer->setPrimitiveRestartEnabled(true);
//	//customMeshRenderer->setRestartIndexValue(65535);
//
//	customMeshRenderer->setGeometry(shpMesh);
//	customMeshRenderer->setVertexCount(shpMesh->getNumVerticies());
//	//customMeshRenderer->setVerticesPerPatch(3);
//
//	customMeshEntity->addComponent(customMeshRenderer);
//	customMeshEntity->addComponent(transform);
//	customMeshEntity->addComponent(material);
//
//	customMeshEntity->setEnabled(true);
//
//	//customMeshEntity->
//	return customMeshEntity;
//}

