#ifndef CITYSIM_H
#define CITYSIM_H

#include <QMainWindow>
#include <QScopedPointer>

namespace Ui {
class citysim;
}

class citysim : public QMainWindow
{
    Q_OBJECT

public:
    explicit citysim(QWidget *parent = nullptr);
    ~citysim() override;

private:
    QScopedPointer<Ui::citysim> m_ui;
};

#endif // CITYSIM_H
