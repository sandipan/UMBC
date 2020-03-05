#ifndef DDMWINDOW_H
#define DDMWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "algo.h"

namespace Ui {
    class DDMWindow;
}

enum GraphicsOperation
{
    NONE,
    DRAW_TREE
};

class DDMWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DDMWindow(QWidget *parent = 0);
    ~DDMWindow();

protected:
    /* *\/
    void paintEvent(QPaintEvent *event);
    /* */

private:
    Ui::DDMWindow *ui;
    GraphicsOperation currentOperation;
    QString dataFile, info;
    Algo algo;

private slots:
    void on_actionPreprocess_Binnary_triggered();
    void on_actionPreproces_EARDMS_triggered();
    void on_action_Preprocess_triggered();
    void on_action_Open_triggered();
};

#endif // DDMWINDOW_H
