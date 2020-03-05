#include <ctime>
#include <QFileDialog>
#include <QMessagebox>
#include "ddmwindow.h"
#include "ui_ddmwindow.h"

#define TREE_ROOT_X 750
#define TREE_ROOT_Y 50
#define TREE_NODE_GAP 250

DDMWindow::DDMWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DDMWindow), info("")
{
    ui->setupUi(this);
    srand(time(NULL));
}

DDMWindow::~DDMWindow()
{
    delete ui;
}

void DDMWindow::on_action_Open_triggered()
{
    if ((dataFile = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("Data (*.data)"))) != QString::null)
    {
        std::string f = dataFile.toLatin1().data();
        algo.RunTests(f.erase(f.find("."), 5));
        QString str = "Classes: ";
        for (int i = 0; i < algo.classes.size(); ++i)
        {
            str += algo.classes[i].c_str();
            str += " ";
        }
        str += "\n";
        std::map<std::string, std::vector<std::string> >::const_iterator it;
        for (it = algo.featurevalues.begin(); it != algo.featurevalues.end(); ++it)
        {
            str += (*it).first.c_str();
            str += ": ";
            std::vector<std::string> v = (*it).second;
            for (int i = 0; i < v.size(); ++i)
            {
                str += v[i].c_str();
                str += " ";
            }
            str += "\n";
        }
        info = str;
        currentOperation = DRAW_TREE;
        /* */
        update();
        /* */
    }
}

/* *\/
void DDMWindow::paintEvent (QPaintEvent *event)
{
    QPainter painter(this);
    if(currentOperation == DRAW_TREE)
    {
        painter.setBrush(Qt::red);
        algo.DrawTree(painter, algo.RDT[0], TREE_ROOT_X, TREE_ROOT_Y, TREE_NODE_GAP);
        painter.drawText(QRectF(10, this->height() - 150, this->width()- 10, 150), info);
    }
    painter.drawText(10, this->height() - 10, algo.status);
}
/* */

void DDMWindow::on_action_Preprocess_triggered()
{
    if ((dataFile = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("Data (*.*)"))) != QString::null)
    {
        algo.PreProcess(dataFile.toLatin1().data());
        QMessageBox::warning(this, "Done",
                             "Done",
                             QMessageBox::Ok,
                             QMessageBox::Ok);
    }
}

void DDMWindow::on_actionPreproces_EARDMS_triggered()
{
    algo.PreProcessEARDMS();
}

void DDMWindow::on_actionPreprocess_Binnary_triggered()
{
    if ((dataFile = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("Data (*.*)"))) != QString::null)
    {
        algo.PreProcessBinary(dataFile.toLatin1().data());
        QMessageBox::warning(this, "Done",
                             "Done",
                             QMessageBox::Ok,
                             QMessageBox::Ok);
    }
}
