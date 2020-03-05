#ifndef MIPSWINDOW_H
#define MIPSWINDOW_H

#include <QtGui/QMainWindow>
#include <queue>
#include <string>
#include "cpu.h"

namespace Ui
{
    class MIPSWindowClass;
}

class MIPSWindow : public QMainWindow
{
    Q_OBJECT

    QString txtEditorText;
    QString fileEdited;
    unsigned int loadAt;
    bool showPipeline;
    CPU cpu;

public:
    MIPSWindow(QWidget *parent = 0);
    ~MIPSWindow();
    char* loadSource(char *file);
    void assembleSource();
    bool loadBinary();
    void writeHalt(unsigned int address);
    void loadRegisters();
    void execute(bool step);
    void showMemory();
    void saveRegisters();
    void parseConfig(char *cfg);
    void MIPSWindow::paintEvent ( QPaintEvent * event );

private:
    Ui::MIPSWindowClass *ui;

private slots:
    void on_bLoadConfig_clicked();
    void on_cShowPipeline_stateChanged(int );
    void on_bStep_clicked();
    void on_bSaveReg_clicked();
    void on_cFPR_currentIndexChanged(int index);
    void on_cGPR_currentIndexChanged(int index);
    void on_bExec_clicked();
    void on_bShow_clicked();
    void on_bAssemble_clicked();
    void on_bLoadSrc_clicked();
};

#endif // MIPSWINDOW_H
