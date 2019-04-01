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

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DInput;
using namespace Qt3DLogic;
using namespace Qt3DExtras;

Qt3DCore::QEntity *loadShp(const QString &path);

MainWindow::MainWindow(const QString &path)
{
	mInitialized = false;

	mRootEntity = createRootEntity();

	mAspectEngine = new QAspectEngine;
	mRenderAspect = new QRenderAspect;
	mInputAspect = new QInputAspect;
	mLogicAspect = new QLogicAspect;
	mRenderSettings = new QRenderSettings;
	mForwardRenderer = new QForwardRenderer;

	mForwardRenderer->setEnabled(true);
	mForwardRenderer->setFrustumCullingEnabled(false);

	mCamera = createCamera();
	mCamera->setParent(mRootEntity);
	mCamController = createCameraController(mRootEntity, mCamera);

	mInputSettings = new QInputSettings;

	mForwardRenderer->setClearColor(QColor(QRgb(0xffffff)));

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

void recurseDirLoad(const QFileInfo info, QEntity *rootEntity);

QString shpPath(const QFileInfo &fi);

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

	if (!path.isEmpty())
	{
		QFileInfo fileInfo(path);
		if (fileInfo.isDir())
		{
			qDebug() << "got dir: " << path;

			QDir dir(path);

			SHPEntity *roadEntity = new SHPEntity;
			if(!roadEntity->load(shpPath(QFileInfo(dir, "roads"))))
			{
				qDebug() << " failed to load roads";
			}

			roadEntity->getTransform()->setScale(0.05f);

			QVector3D roadOrigin = roadEntity->origin() * 0.05f;

			roadEntity->setParent(mRootEntity);

			SHPEntity *rooflineEntity = new SHPEntity;
			if(!rooflineEntity->load(shpPath(QFileInfo(dir, "rooflines"))))
			{
				qDebug() << " failed to load rooflines";
			}

			QVector3D rooflineOrigin = rooflineEntity->origin() * 0.05f;
			QVector3D diffOrigin = rooflineOrigin - roadOrigin;

			qDebug() << " roofline orig: " << rooflineOrigin << " road orig: " << roadOrigin;
			qDebug() << " diff orig: " << diffOrigin;

			Qt3DCore::QTransform *rooflineTransform = rooflineEntity->getTransform();
			rooflineTransform->setTranslation(diffOrigin);
			rooflineTransform->setScale(0.05f);

			rooflineEntity->setParent(mRootEntity);
		}
	}

	return mRootEntity;
}

QString shpPath(const QFileInfo &fi)
{
	if(!fi.isDir())
	{
		qDebug() << " not a dir!";
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
			qDebug() << " found: " << fileInfo.filePath();
			return fileInfo.filePath();
		}
	}

	return "";
}

void dirLoad(const QString &dirName, QEntity *rootEntity)
{
	qDebug() << "dirLoad: " << dirName;

	QEntity *shpEntity = loadShp(dirName);
	shpEntity->setParent(rootEntity);
}

void recurseDirLoad(const QFileInfo info, QEntity *rootEntity)
{
	qDebug() << "recurseDirLoad: " << info;

	if(!info.isDir())
	{
		qDebug() << " not a dir!";
		return;
	}

	QDir dir(info.filePath());
	qDebug() << " dir: " << dir;

	QFileInfoList fileInfoList = dir.entryInfoList(
			QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);

	qDebug() << " infoList: " << fileInfoList;

	for (QFileInfo entInfo: fileInfoList)
	{
		qDebug() << " entry: " << entInfo;

		if(entInfo.isDir())
		{
			recurseDirLoad(entInfo, rootEntity);
		}
		else
		{
			//qDebug() << " file: suffix=" << entInfo.suffix();
			if (entInfo.suffix() == "shp" || entInfo.suffix() == "shx")
			{
				QFileInfo fileInfo(entInfo.dir(), entInfo.baseName());

				dirLoad(fileInfo.filePath(), rootEntity);
				return;
			}
		}
	}
}

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
	QEntity *entity = new QEntity;
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

void MainWindow::resizeEvent(QResizeEvent *event)
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


Qt3DCore::QEntity *loadShp(const QString &path)
{
	qDebug() << "loadShp: " << path;

	Qt3DCore::QEntity *customMeshEntity = new Qt3DCore::QEntity();
	customMeshEntity->setObjectName("customMeshEntity");

	Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
	//transform->setScale(0.01f); // ??
	//transform->setRotationY(rot);
	transform->setTranslation(QVector3D(0, 0, 0.0f));

	QMaterial *material = new QPerVertexColorMaterial(customMeshEntity);

	QGeometryRenderer *customMeshRenderer = new Qt3DRender::QGeometryRenderer();

	SHPMeshGeometry *shpMesh = new SHPMeshGeometry(path, customMeshRenderer);

	customMeshRenderer->setInstanceCount(1);
	customMeshRenderer->setIndexOffset(0);
	customMeshRenderer->setFirstInstance(0);
	customMeshRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
	//customMeshRenderer->setPrimitiveRestartEnabled(true);
	//customMeshRenderer->setRestartIndexValue(65535);

	customMeshRenderer->setGeometry(shpMesh);
	customMeshRenderer->setVertexCount(shpMesh->getNumVerticies());
	//customMeshRenderer->setVerticesPerPatch(3);

	customMeshEntity->addComponent(customMeshRenderer);
	customMeshEntity->addComponent(transform);
	customMeshEntity->addComponent(material);

	customMeshEntity->setEnabled(true);

	//customMeshEntity->
	return customMeshEntity;
}

