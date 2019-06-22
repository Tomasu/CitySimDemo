//
// Created by moose on 27/03/19.
//

#include "MainWindow.h"
#include "graph/TransportGraph.h"

#include "quadtree/QuadTreeNodeEntityFactory.h"
#include "quadtree/QuadTreeNodeEntity.h"
#include "quadtree/QuadTreeNode.h"
#include "quadtree/QuadTree.h"
#include "aspect/FpsMonitorAspect.h"
#include "aspect/FpsMonitorComponent.h"
#include "RoadData.h"

#include "RootEntity.h"

#include "Constants.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <QtGui/QOpenGLContext>

#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DInput/QInputAspect>
#include <Qt3DInput/QInputSettings>
#include <Qt3DInput/QMouseHandler>
#include <Qt3DInput/QMouseDevice>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QPointLight>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QCullFace>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QLineWidth>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>

#include <Qt3DLogic/QLogicAspect>
#include <Qt3DLogic/QFrameAction>

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QFirstPersonCameraController>
#include <Qt3DExtras/QPerVertexColorMaterial>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QForwardRenderer>

#include <Qt3DRender/QScreenRayCaster>

#include <ogrsf_frmts.h>

#include <shapefil.h>
#include <iostream>
#include <memory.h>
#include <string.h>

#include "LogUtils.h"


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
			QuadTreeNodeEntity *newEntity = new QuadTreeNodeEntity(node, mMainWindow, rootNode());
			//newEntity->setParent(rootNode());
			log_trace_exit();
			return newEntity;
		}

	private:
		MainWindow *mMainWindow;
};

class MainQuadTreeNodeMapper : public QuadTreeNodeMapper
{
	private:
		MainWindow *mMainWindow;

	public:
		MainQuadTreeNodeMapper(MainWindow *mw)
			: mMainWindow{mw}
		{
			// nada
		}

		~MainQuadTreeNodeMapper()
		{
			// nada
		}

		QVector3D map(const QVector3D &vec)
		{
			QMatrix4x4 matrix = mMainWindow->camera()->viewMatrix();
			return matrix.mapVector(vec);
		}
};

Qt3DCore::QEntity *loadShp(const QString &path);

MainWindow::MainWindow(const QString &path)
	: mRoadsQuadTree{nullptr}
{
	mInitialized = false;
	mMouseMoved = true;

	mNodeMapper = new MainQuadTreeNodeMapper(this);

	mGraph = new TransportGraph();

	mRootEntity = createRootEntity();

	mAspectEngine = new QAspectEngine;
	mRenderAspect = new QRenderAspect();
	mInputAspect = new QInputAspect;
	mLogicAspect = new QLogicAspect;
	mRenderSettings = new QRenderSettings;
	mRenderSettings->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::BoundingVolumePicking);
	mRenderSettings->pickingSettings()->setPickResultMode(Qt3DRender::QPickingSettings::AllPicks);
	mForwardRenderer = new QForwardRenderer;

	mForwardRenderer->setEnabled(true);
	mForwardRenderer->setFrustumCullingEnabled(false);

	mCamera = createCamera(mRootEntity);
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
	format.setOption(QSurfaceFormat::DebugContext);

	QSurfaceFormat::setDefaultFormat(format);
	setFormat(format);

	mAspectEngine->registerAspect(mLogicAspect);
	mAspectEngine->registerAspect(mInputAspect);
	mAspectEngine->registerAspect(mRenderAspect);
	mAspectEngine->registerAspect(mFpsAspect);

//	QLineWidth *lineWidth = new QLineWidth(mRenderSettings);
//	lineWidth->setValue(15.0f);

	mForwardRenderer->setCamera(mCamera);
	mForwardRenderer->setSurface(this);
	mRenderSettings->setActiveFrameGraph(mForwardRenderer);

	mInputSettings->setEventSource(this);

	mMouseDevice = new QMouseDevice(mRootEntity);
	mMouseHandler = new QMouseHandler();
	mMouseHandler->setSourceDevice(mMouseDevice);
	QObject::connect(mMouseHandler, &QMouseHandler::positionChanged, this, &MainWindow::mousePositionChanged);

// 	mRayCaster = new QScreenRayCaster(mRootEntity);
// 	mRayCaster->setRunMode(Qt3DRender::QAbstractRayCaster::Continuous);
// 	mRootEntity->addComponent(mRayCaster);
//
// 	QObject::connect(mRayCaster, &QScreenRayCaster::hitsChanged, this, [&](const Qt3DRender::QAbstractRayCaster::Hits &hits) {
// 		for (Qt3DRender::QRayCasterHit hit: hits)
// 		{
// 			QVector3D worldIntersection = hit.worldIntersection();
// 			QVector3D local = hit.localIntersection();
// 			QuadTreeNodeEntity *nodeEntity = dynamic_cast<QuadTreeNodeEntity*>(hit.entity());
// 			if (nodeEntity == nullptr)
// 			{
// 				log_debug("hit without a node?!");
// 				continue;
// 			}
//
// 			if (nodeEntity->node()->bounds().contains(local.x(), local.y()))
// 			{
// 				log_debug("hit: %s %s", nodeEntity->node()->bounds(), hit.localIntersection());
// 			}
// 		}
// 	});

	createScene(mRootEntity, path);

	//mCamera->viewEntity(mMapEntity);
	mRootEntity->addComponent(mMouseHandler);

	mFrameAction = new Qt3DLogic::QFrameAction(mRootEntity);
	mRootEntity->addComponent(mFrameAction);

	QObject::connect(mFrameAction, &Qt3DLogic::QFrameAction::triggered, this, &MainWindow::frameActionTriggered);
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

	delete mGraph;
	delete mRoadsQuadTree;
	delete mNodeMapper;
	delete mEntityFactory;
}

//void recurseDirLoad(const QFileInfo info, QEntity *rootEntity);

void MainWindow::frameActionTriggered(float dt)
{
	if (!mMouseMoved)
	{
		return;
	}

	mMouseMoved = false;

	log_debug("camera transform: %s", mCamera->transform());
	log_debug("camera viewMatrix: %s", mCamera->viewMatrix());
	log_debug("camera projMatrix: %s", mCamera->projectionMatrix());

	QRect viewport = QRect{QPoint{0,0}, QPoint(width(), height())};

	QMatrix4x4 viewMatrix = mCamera->viewMatrix();
	QMatrix4x4 modelViewMatrix = viewMatrix * mMapTransform->matrix();
	QMatrix4x4 modelViewProject = mCamera->projectionMatrix() * modelViewMatrix;

	QMatrix4x4 matrix = modelViewProject.inverted();

	Rect worldRect = mRoadsQuadTree->bounds();
	log_debug("bounds: %s", worldRect);

	//mCamera->viewMatrix() * QVector3D{ev->x(), ev->y(), 0.0f};// mCamera->projectionMatrix();
	//QVector3D mapped = matrix.map({(float)ev->x(), (float)ev->y(), -mCamera->position().z()});

	QVector3D camPos = mCamera->position();
	Qt3DCore::QTransform *camTransform = mCamera->transform();
	QVector3D camTranslation = camTransform->translation();

	log_debug("cam: pos=%s, translation=%s", camPos, camTranslation);
	log_debug("cam ex: left=%s top=%s right=%s bottom=%s", mCamera->left(), mCamera->top(), mCamera->right(), mCamera->bottom());

	float camZ = mCamera->position().z();
	float znear = mCamera->nearPlane();
	float zfar = mCamera->farPlane();
	log_debug("znear: %s zfar: %s", znear, zfar);

	// 		QMatrix4x4 viewInverted = (mCamera->projectionMatrix() * mCamera->viewMatrix()).inverted();
	// 		QVector3D rootEntityOrigin = viewInverted.map(mRootTransform->translation());
	//
	// 		log_debug("rootEntityOrigin: %s (%s)", rootEntityOrigin, mRootTransform->translation());
	//
	// 		QVector3D mapEntityOrigin = viewInverted.mapVector(mMapTransform->translation());
	// 		log_debug("mapEntityOrigin: %s (%s)", mapEntityOrigin, mMapTransform->translation());
	//
	// 		QMatrix4x4 viewMatrix2 = mCamera->viewMatrix();
	// 		QRectF rootRect = viewMatrix2.mapRect(worldRect);
	// 		log_debug("rootRect: %s", rootRect);

	// 		int halfWidth = width()/2;
	// 		int halfHeight = height()/2;
	// 		int left = -halfWidth;
	// 		int top = -halfHeight;
	// 		int right = halfWidth;
	// 		int bottom = halfHeight;
	// 		QRect viewport = QRect{QPoint{0,0}, QPoint(width(), height())};
	//
	//
	// 		float camZ = mCamera->position().z();
	//
	// 		float znear = mCamera->nearPlane();
	// 		float zfar = mCamera->farPlane();
	// 		float zvalue = znear + (zfar - znear) * 0.01f; // z-depth
	// 		log_debug("zvalue: %s", zvalue);
	// 		QVector4D mouseVec = QVector4D{(float)ev->x(), ((float)height() - (float)ev->y()), 1.0f, 1.0f};
	// 		log_debug("mouseVec: %s", mouseVec.toVector3D());
	//
	// 		QMatrix4x4 mvp = (mCamera->viewMatrix() * mCamera->projectionMatrix()).inverted();
	// 		QVector3D mouseMvp = (mouseVec * mvp).toVector3D();
	// 		log_debug("mouseMvp: %s", mouseMvp);
	//
	// 		QVector3D unprojected = mouseVec.toVector3D().unproject(modelViewMatrix, mCamera->projectionMatrix(), viewport);
	// 		QVector3D projected = mouseVec.toVector3D().project(modelViewMatrix, mCamera->projectionMatrix(), viewport);
	// 		log_debug("unprojected: %s %s", unprojected, unprojected + mapEntityOrigin);
	// 		log_debug("projected: %s", projected);

	QVector4D mouseVec4d = QVector4D{(float)mMousePos.x(), height() - (float)mMousePos.y() - 1.0f, 1.0f, 1.0f};
	QVector4D mapped = modelViewProject.inverted() * mouseVec4d;

	//QVector3D mapped = matrix.transposed() * QVector3D{(float)ev->x(), (float)ev->y(), -mCamera->position().z()};
	//QVector3D mapped = matrix.map(QVector3D{(float)ev->x(), /*height() - */(float)ev->y(), mCamera->position().z()});


	log_debug("pos=%sx%s -> %s", mMousePos.x(), mMousePos.y(), mapped.toVector3D());

	QVector3D mouseVec3d = QVector3D{(float)mMousePos.x(), (float)height() - (float)mMousePos.y() - 1.0f, 0.0f};

	QMatrix4x4 camViewMatrix = mCamera->viewMatrix();
	QMatrix4x4 camProjMatrix = mCamera->projectionMatrix();
	QMatrix4x4 rootMatrix = mRootTransform->matrix();
	QMatrix4x4 mapMatrix = mMapTransform->matrix();

	QVector3D worldMousePos = QVector3D{(float)mMousePos.x(), (float)height() - (float)mMousePos.y() - 1.0f, 0.0f};
	QMatrix4x4 worldMatrix = camProjMatrix * camViewMatrix * mapMatrix;
	QVector3D worldMapped = worldMatrix.inverted().map(worldMousePos);
	log_debug("worldMapped: %s", worldMapped);

	QVector3D vv = mouseVec3d * camViewMatrix.inverted();
	log_debug("vv -> %s x=%s y=%s z=%s", vv, vv.x(), vv.y(), vv.z());

	QMatrix4x4 mm = mapMatrix * rootMatrix * camViewMatrix * camProjMatrix.inverted();
	QVector3D vv2 = mouseVec3d * mm;
	log_debug("vv2 -> %s", vv2);

	float nmx = ((float)mMousePos.x()*2 / width()) - 1.0f;
	float nmy = 1.0f - ((float)(height()-mMousePos.y()) * 2) / height();

	QVector4D mouseNormDevCoords = QVector4D{nmx, nmy, -1.0f, 1.0f};
	log_debug("mndc=%s", mouseNormDevCoords);

	QMatrix4x4 camProjViewMatrix = camProjMatrix * camViewMatrix * rootMatrix * mapMatrix;
	QVector4D convNdvc = camProjViewMatrix * mouseNormDevCoords;

	float w = convNdvc.w();
	QVector3D vv3 = QVector3D{convNdvc.x() / w, convNdvc.y() / w, convNdvc.z() / w};

	log_debug("vv3: %s", vv3);

	QVector4D vv4 = camProjViewMatrix * mouseNormDevCoords;
	log_debug("vv4: %s", vv4);

	QVector4D vv5 = vv4 / vv4.z();
	log_debug("vv5: %s", vv5);

	//log_debug("vv6: %s", rootMatrix * mapMatrix * vv5);

	log_debug("root coords: %s", mRootTransform->translation());
	log_debug("map coords: %s", mMapTransform->translation());

	QVector3D upMousePos = QVector3D{(float)mMousePos.x(), (float)height() - (float)mMousePos.y() - 1.0f, 1.0f};
	QVector3D upMousePosFar = QVector3D{(float)mMousePos.x(), (float)height() - (float)mMousePos.y() - 1.0f, 0.0f};

	QMatrix4x4 identity;
	QVector3D minProj = QVector3D(0,0,1.0f).unproject(camViewMatrix*mapMatrix, camProjMatrix, viewport);
	QVector3D midProj = QVector3D(width()/2,height()/2,1).unproject(camViewMatrix*mapMatrix, camProjMatrix, viewport);
	QVector3D maxProj = QVector3D(width()-1,height()-1,1).unproject(camViewMatrix*mapMatrix, camProjMatrix, viewport);

	log_debug("min: %s mid: %s max: %s", minProj, midProj, maxProj);

	QVector3D minMinusMax = minProj - maxProj;
	log_debug("minMinusMax: %s", minMinusMax);

	QMatrix4x4 upMvM = mapMatrix * camViewMatrix;
	QVector3D unproj = upMousePos.unproject(upMvM, camProjMatrix, viewport);
	QVector3D unprojFar = upMousePosFar.unproject(upMvM, camProjMatrix, viewport);

	log_debug("unproj: %s far: %s", unproj, unprojFar);

	QVector3D unprojDiff = unproj - unprojFar;
	log_debug("unprojDiff: %s", unprojDiff);

	float realz = 1.0f / (znear-zfar) * -camZ;
	log_debug("realz: %s", realz);

	QVector3D rempos = QVector3D{(float)mMousePos.x(), height() -(float)mMousePos.y(), realz};
	QVector3D reunproj = rempos.unproject(camViewMatrix, camProjMatrix, viewport);
	log_debug("reunproj: %s %s", reunproj, mapMatrix * reunproj);

	QVector3D unprojfff = (unprojDiff / (znear-zfar)) * -mCamera->position().z();
	log_debug("unprojfff: %s", unprojfff);

	//unprojfff.setZ(mCamera->position().z());
	QVector4D unprojTrans = mapMatrix * QVector4D{unprojfff, 1.0f};//(rootMatrix*mapMatrix.inverted()).map(unproj);
	log_debug("unprojTrans: %s", unprojTrans);

	QuadTreeNode *node = mRoadsQuadTree->findNode(unprojTrans.toVector3D());
	if (node != nullptr)
	{
		log_debug("\t-> in node %s", node->bounds());
	}
	// 		QuadTreeNode *node = mRoadsQuadTree->findNode(mapped.toVector3D());
	// 		if (node != nullptr)
	// 		{
	// 			log_debug("\t-> in node %s", node->bounds());
	// 		}

	//QRectF bounds = mNode->bounds();
	//log_debug("bounds: %sx%s %sx%s ms: %sx%s", bounds.left(), bounds.top(), bounds.right(), bounds.bottom(), mapped.x(), mapped.y());
	// 		if (mapped.x() >= bounds.left()
	// 			&& mapped.x() <= bounds.right()
	// 			&& mapped.y() >= bounds.top()
	// 			&& mapped.y() <= bounds.bottom()
	// 		)
	// 		{
	// 			log_debug("%s ev: %s mapped: %s", bounds, ev, mapped);
	// 		}
}

void MainWindow::mousePositionChanged(Qt3DInput::QMouseEvent* ev)
{
	//if (mMouseHandler->containsMouse())
	//if(0)
	{
		mMousePos = QPoint{ev->x(), ev->y()};
		//mMouseMoved = true;

		//mRayCaster->trigger(mMousePos);
		//mRayCaster->setPosition(mMousePos);

	}
}


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

bool isReverseEdge(const char *str)
{
	return strcmp(str, DIGITZ_1R_ONEWAY_REV) == 0
		|| strcmp(str, DIGITZ_2R_TRUE_ONEWAY_REV) == 0;
}

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

void MainWindow::buildRoadGraph(Qt3DCore::QEntity *parentEntity, GDALDataset *dataset)
{
	log_trace_enter();

	OGRLayer *layer = dataset->GetLayer(0);

	layer->ResetReading();

	OGRFeature *poFeature = nullptr;

	OGRSpatialReference *layerSpatialRef = layer->GetSpatialRef();

	OGRSpatialReference *mercSrs = new OGRSpatialReference();
	OGRErr retr = mercSrs->importFromEPSG(3776);
	if (retr != OGRERR_NONE)
	{
		log_error("couldnt get well known CS for transform... ");
		return;
	}

	OGRCoordinateTransformation *coordTransform = OGRCreateCoordinateTransformation(layerSpatialRef, mercSrs);

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

	Rect extRect = Rect(Point(extentX[0], extentY[0]), Point(extentX[1], extentY[1]));

	log_debug("extRect: %s", extRect);


	Rect adjRect = extRect;
	adjRect.moveTo(0, 0);

	log_debug("adjRect: %s", adjRect);

	//mMapTransform->setTranslation(QVector3D{-(float)adjRect.width()/32.0f, /*(float)adjRect.height()/2.0f*/0.0f, 0.0f});

	Point origin = adjRect.bottomRight();
	//mMapTransform->setTranslation({-(float)origin.x(), -(float)origin.y(), 0.0f});

	mRoadsQuadTree = new QuadTree(mEntityFactory, adjRect, mNodeMapper);

	typedef std::pair<double,double> DoublePair;
	std::map<DoublePair, TransportGraph::Vertex> vertices;

	Point topLeft = extRect.topLeft();
	QVector3D offsetp = QVector3D{(float)topLeft.x(), (float)topLeft.y(), BASE_Z_OFFSET};

	while ((poFeature = layer->GetNextFeature()))
	{
		// class_cd & dititz
		int classCdIdx = poFeature->GetFieldIndex("class_cd");
		int digitzIdx = poFeature->GetFieldIndex("digitiz_cd");
		int nameAbPrIdx = poFeature->GetFieldIndex("name_ab_pr");

		const char *classCd = strdup(poFeature->GetFieldAsString(classCdIdx));
		const char *digitz = strdup(poFeature->GetFieldAsString(digitzIdx));
		const char *nameAbPr = strdup(poFeature->GetFieldAsString(nameAbPrIdx));

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

		float geomMinX = geomEnvelope.MinX - extRect.left();
		float geomMaxX = geomEnvelope.MaxX - extRect.left();
		float geomMinY = geomEnvelope.MinY - extRect.top();
		float geomMaxY = geomEnvelope.MaxY - extRect.top();

		Rect geomBounds{
			geomMinX, geomMinY,
			geomMaxX - geomMinX, geomMaxY - geomMinY
		};

		//log_debug("geomBounds: %s", geomBounds);

		RoadData *itemData = new RoadData(geomBounds, nameAbPr, classCd);

		TransportGraph::Vertex vtx1;

		int numPoints = lineString->getNumPoints();
		for (int i = 0; i < numPoints-1; ++i)
		{
			OGRPoint startPt;
			lineString->getPoint(i, &startPt);

			OGRPoint endPt;
			lineString->getPoint(i+1, &endPt);

			float sx = startPt.getX();
			float sy = startPt.getY();
			float sz = BASE_Z_OFFSET;//startPt.getZ();

			float ex = endPt.getX();
			float ey = endPt.getY();
			float ez = BASE_Z_OFFSET;//endPt.getZ();

			QVector3D qsp = QVector3D(sx, sy, sz) - offsetp;
			QVector3D qep = QVector3D(ex, ey, ez) - offsetp;

			auto vtx1it = vertices.find(std::make_pair(qsp.x(), qsp.y()));
			if (vtx1it == vertices.end())
			{
				vtx1 = mGraph->addVertex(vertices.size(), qsp);
			}
			else
			{
				vtx1 = vtx1it->second;
			}

			TransportGraph::Vertex vtx2;
			auto vtx2it = vertices.find(std::make_pair(qep.x(), qep.y()));
			if (vtx2it == vertices.end())
			{
				vtx2 = mGraph->addVertex(vertices.size(), qep);
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
				itemData->addFwdEdge(forwardEdge, qsp, qep);
			}

			if (makeReverseEdge)
			{
				TransportGraph::Edge reverseEdge = mGraph->addEdge(poFeature->GetFID(), vtx2, vtx1);
				itemData->addRevEdge(reverseEdge, qep, qsp);
			}

			if (isReverseEdge(digitz))
			{
				log_debug("got rev road: %s %s", nameAbPr, digitz);
				itemData->addSegment(qep, qsp);
			}
			else
			{
				itemData->addSegment(qsp, qep);
			}
		}

		free((void*)nameAbPr);
		free((void*)digitz);
		free((void*)classCd);

		mRoadsQuadTree->insertItem(itemData, true);

		OGRFeature::DestroyFeature(poFeature);
	}

	OCTDestroyCoordinateTransformation(coordTransform);
	mercSrs->Dereference();
	OGRSpatialReference::DestroySpatialReference(mercSrs);
	//delete mercSrs;

	mRoadsQuadTree->dumpTree();
	mRoadsQuadTree->updateGeometry();

	log_trace_exit();
}

void MainWindow::createScene(Qt3DCore::QEntity *rootEntity, const QString &path)
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

	QEntity *lightEntity = new QEntity(rootEntity);
	lightEntity->setObjectName("lightEntity");
	QPointLight *light = new QPointLight(lightEntity);
	light->setColor("white");
	light->setIntensity(1);
	lightEntity->addComponent(light);

	Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(lightEntity);
	lightTransform->setTranslation(mCamera->position());
	lightEntity->addComponent(lightTransform);

	FpsMonitorComponent *fpsMonitor = new FpsMonitorComponent(rootEntity);
	fpsMonitor->setRollingMeanFrameCount(60);
//	connect(fpsMonitor, &FpsMonitorComponent::framesPerSecondChanged, [&](float rmfc) {
//		std::cout << " fps: " << rmfc << std::endl;
//	});

	rootEntity->addComponent(fpsMonitor);

	mMapEntity = new QEntity(rootEntity);
	mMapEntity->setObjectName("mapEntity");
	mMapTransform = new Qt3DCore::QTransform(mMapEntity);
	mMapTransform->setScale(0.2f);
	mMapEntity->addComponent(mMapTransform);

	Qt3DRender::QObjectPicker *picker = new Qt3DRender::QObjectPicker(mMapEntity);
	picker->setHoverEnabled(true);
	picker->setDragEnabled(true);

	QObject::connect(picker, &QObjectPicker::moved, this, [&](Qt3DRender::QPickEvent *ev) {
		log_debug("ev: pos: %s dist: %s", ev->position(), ev->distance());
		log_debug("ev: local: %s", ev->localIntersection());
		log_debug("ev: world: %s", mMapTransform->matrix().inverted() * ev->worldIntersection());

		QMatrix4x4 mm = (mCamera->viewMatrix() * mMapTransform->matrix()).inverted();
		log_debug("ev world-mod: %s", mm * ev->worldIntersection());

		QuadTreeNode *node = mRoadsQuadTree->findNode(ev->localIntersection());
		if (node != nullptr)
		{
			log_debug("got node! %s", node->bounds());
		}

	});

	mMapEntity->addComponent(picker);

	mEntityFactory = new MainQuadTreeNodeEntityFactory(this, mMapEntity);

// 	QMouseDevice *mouseDevice = new QMouseDevice(mRootEntity);
// 	QMouseHandler *mouseHandler = new QMouseHandler(mRootEntity);
// 	mouseHandler->setSourceDevice(mouseDevice);
// 	mRootEntity->addComponent(mouseHandler);
//
// 	QObject::connect(mouseHandler, &QMouseHandler::positionChanged, this, [&](Qt3DInput::QMouseEvent *ev){
// 		log_debug("mouse move: %sx%s", ev->x(), ev->y());
// 	});

	//mRootTransform = new Qt3DCore::QTransform(mRootEntity);
	//mRootTransform->setTranslation({0.0f, 0.0f, 1.0f});
	//mRootEntity->addComponent(mRootTransform);

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

			buildRoadGraph(mMapEntity, roadsDataset);

			GDALClose(roadsDataset);
			GDALDestroy();

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
//bounds().bottomRight()
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

QCamera *MainWindow::createCamera(Qt3DCore::QEntity *rootEntity)
{
	// Camera
	QCamera *cameraEntity = new QCamera(rootEntity);
	cameraEntity->setObjectName("camera");
	cameraEntity->lens()->setPerspectiveProjection(45.0f, 1024.0f/768.0f, Z_NEAR, Z_FAR);
	cameraEntity->setPosition(QVector3D(0, 0, 4000.0f));
	cameraEntity->setUpVector(QVector3D(0, 1, 0));
	cameraEntity->setViewCenter(QVector3D(0, 0, 0));

	//QObject::connect(cameraEntity, &QCamera::positionChanged, this, &MainWindow::frameActionTriggered);

	return cameraEntity;
}

QAbstractCameraController *MainWindow::createCameraController(QEntity *rootEntity, QCamera *camera)
{
//	QOrbitCameraController *camController = new QOrbitCameraController(rootEntity);

	auto *camController = new QFirstPersonCameraController(rootEntity);
	camController->setObjectName("cameraController");
	camController->setLinearSpeed(90.0f);
	camController->setLookSpeed(180.0f);
	camController->setCamera(camera);

	return camController;
}

Qt3DCore::QEntity *MainWindow::createRootEntity()
{
	QEntity *entity = new RootEntity;
	entity->setObjectName("rootEntity");

	mRootTransform = new Qt3DCore::QTransform(entity);
	mRootTransform->setScale(1.0f);
	//entity->addComponent(mRootTransform);

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

Qt3DCore::QEntity * MainWindow::mapEntity()
{
	return mMapEntity;
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

