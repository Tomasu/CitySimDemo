#include "citysim.h"
#include "ui_citysim.h"

citysim::citysim(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::citysim)
{
    m_ui->setupUi(this);
}

citysim::~citysim() = default;
