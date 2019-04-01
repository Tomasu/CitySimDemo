#include "MainWindow.h"
#include "ShpMeshGeometry.h"
#include <QApplication>
#include <Qt3DExtras/QForwardRenderer>

#include <QtDebug>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QMaterial>
#include <Qt3DExtras/QPerVertexColorMaterial>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QAttribute>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DInput/QInputAspect>
#include <Qt3DRender/QLineWidth>
#include <Qt3DRender/QPointLight>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QPhongMaterial>

#include <ogrsf_frmts.h>

int main(int argc, char *argv[])
{
	char *path = argc > 0 ? argv[1] : nullptr;

	GDALAllRegister();

	QGuiApplication app(argc, argv);
	MainWindow w(path);

	w.show();

	return app.exec();
}



