#include "MainWindow.h"
#include <QGuiApplication>

#define TAG "main"
#include "util/LogUtils.h"


int main(int argc, char *argv[])
{
	char *path = argc > 0 ? argv[1] : nullptr;


	GDALAllRegister();

	QGuiApplication app(argc, argv);
	MainWindow w(path);

	w.show();

	return app.exec();
}



