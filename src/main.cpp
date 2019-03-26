#include "citysim.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    citysim w;
    w.show();

    return app.exec();
}

