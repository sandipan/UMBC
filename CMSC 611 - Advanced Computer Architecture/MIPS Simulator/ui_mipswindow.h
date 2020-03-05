/********************************************************************************
** Form generated from reading ui file 'mipswindow.ui'
**
** Created: Wed Nov 4 22:58:35 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MIPSWINDOW_H
#define UI_MIPSWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QTextEdit>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MIPSWindowClass
{
public:
    QWidget *centralWidget;
    QGroupBox *gCompilerLoader;
    QTextEdit *tEditor;
    QPushButton *bLoadSrc;
    QPushButton *bAssemble;
    QPushButton *bLoadConfig;
    QLabel *lLoadAt;
    QLineEdit *tLoad;
    QGroupBox *gMemory;
    QLabel *lStartLoc;
    QLineEdit *tLoc;
    QPushButton *bShow;
    QTextEdit *tMemory;
    QPushButton *bSave_2;
    QGroupBox *gRegister;
    QLineEdit *tLMD;
    QComboBox *cFPR;
    QLineEdit *tPC;
    QLabel *lFPR;
    QPushButton *bExec;
    QLineEdit *tIR;
    QLabel *lLMD;
    QLabel *lIR;
    QLabel *lPC;
    QLineEdit *tFPR;
    QLabel *lGPR;
    QLineEdit *tOp;
    QComboBox *cGPR;
    QLabel *lOp;
    QLineEdit *tGPR;
    QLineEdit *tImm;
    QLabel *lImm;
    QLineEdit *tB;
    QLabel *lB;
    QLineEdit *tA;
    QLabel *lA;
    QLineEdit *tALU;
    QLabel *lALU;
    QPushButton *bSaveReg;
    QPushButton *bStep;
    QGroupBox *gResult;
    QTextEdit *tResults;
    QGroupBox *gName;
    QFrame *line_4;
    QLabel *lName;
    QFrame *line;
    QFrame *line_2;
    QFrame *line_3;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MIPSWindowClass)
    {
        if (MIPSWindowClass->objectName().isEmpty())
            MIPSWindowClass->setObjectName(QString::fromUtf8("MIPSWindowClass"));
        MIPSWindowClass->resize(1259, 677);
        MIPSWindowClass->setMinimumSize(QSize(0, 0));
        centralWidget = new QWidget(MIPSWindowClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gCompilerLoader = new QGroupBox(centralWidget);
        gCompilerLoader->setObjectName(QString::fromUtf8("gCompilerLoader"));
        gCompilerLoader->setGeometry(QRect(0, 10, 251, 231));
        tEditor = new QTextEdit(gCompilerLoader);
        tEditor->setObjectName(QString::fromUtf8("tEditor"));
        tEditor->setGeometry(QRect(10, 20, 231, 141));
        bLoadSrc = new QPushButton(gCompilerLoader);
        bLoadSrc->setObjectName(QString::fromUtf8("bLoadSrc"));
        bLoadSrc->setGeometry(QRect(10, 170, 111, 23));
        bAssemble = new QPushButton(gCompilerLoader);
        bAssemble->setObjectName(QString::fromUtf8("bAssemble"));
        bAssemble->setGeometry(QRect(10, 200, 111, 23));
        bLoadConfig = new QPushButton(gCompilerLoader);
        bLoadConfig->setObjectName(QString::fromUtf8("bLoadConfig"));
        bLoadConfig->setGeometry(QRect(130, 170, 111, 23));
        lLoadAt = new QLabel(gCompilerLoader);
        lLoadAt->setObjectName(QString::fromUtf8("lLoadAt"));
        lLoadAt->setGeometry(QRect(130, 200, 41, 16));
        tLoad = new QLineEdit(gCompilerLoader);
        tLoad->setObjectName(QString::fromUtf8("tLoad"));
        tLoad->setGeometry(QRect(180, 200, 61, 20));
        gMemory = new QGroupBox(centralWidget);
        gMemory->setObjectName(QString::fromUtf8("gMemory"));
        gMemory->setGeometry(QRect(260, 10, 241, 231));
        lStartLoc = new QLabel(gMemory);
        lStartLoc->setObjectName(QString::fromUtf8("lStartLoc"));
        lStartLoc->setGeometry(QRect(10, 30, 46, 20));
        tLoc = new QLineEdit(gMemory);
        tLoc->setObjectName(QString::fromUtf8("tLoc"));
        tLoc->setGeometry(QRect(50, 30, 61, 20));
        bShow = new QPushButton(gMemory);
        bShow->setObjectName(QString::fromUtf8("bShow"));
        bShow->setGeometry(QRect(120, 30, 51, 23));
        tMemory = new QTextEdit(gMemory);
        tMemory->setObjectName(QString::fromUtf8("tMemory"));
        tMemory->setGeometry(QRect(10, 60, 221, 161));
        bSave_2 = new QPushButton(gMemory);
        bSave_2->setObjectName(QString::fromUtf8("bSave_2"));
        bSave_2->setEnabled(false);
        bSave_2->setGeometry(QRect(180, 30, 51, 23));
        gRegister = new QGroupBox(centralWidget);
        gRegister->setObjectName(QString::fromUtf8("gRegister"));
        gRegister->setGeometry(QRect(520, 10, 261, 231));
        tLMD = new QLineEdit(gRegister);
        tLMD->setObjectName(QString::fromUtf8("tLMD"));
        tLMD->setEnabled(false);
        tLMD->setGeometry(QRect(170, 200, 71, 20));
        cFPR = new QComboBox(gRegister);
        cFPR->setObjectName(QString::fromUtf8("cFPR"));
        cFPR->setGeometry(QRect(40, 170, 51, 22));
        tPC = new QLineEdit(gRegister);
        tPC->setObjectName(QString::fromUtf8("tPC"));
        tPC->setGeometry(QRect(40, 20, 71, 20));
        lFPR = new QLabel(gRegister);
        lFPR->setObjectName(QString::fromUtf8("lFPR"));
        lFPR->setGeometry(QRect(10, 170, 20, 21));
        bExec = new QPushButton(gRegister);
        bExec->setObjectName(QString::fromUtf8("bExec"));
        bExec->setGeometry(QRect(120, 20, 61, 23));
        tIR = new QLineEdit(gRegister);
        tIR->setObjectName(QString::fromUtf8("tIR"));
        tIR->setEnabled(false);
        tIR->setGeometry(QRect(40, 50, 201, 20));
        lLMD = new QLabel(gRegister);
        lLMD->setObjectName(QString::fromUtf8("lLMD"));
        lLMD->setGeometry(QRect(140, 200, 21, 21));
        lIR = new QLabel(gRegister);
        lIR->setObjectName(QString::fromUtf8("lIR"));
        lIR->setGeometry(QRect(10, 50, 21, 20));
        lPC = new QLabel(gRegister);
        lPC->setObjectName(QString::fromUtf8("lPC"));
        lPC->setGeometry(QRect(10, 20, 21, 20));
        tFPR = new QLineEdit(gRegister);
        tFPR->setObjectName(QString::fromUtf8("tFPR"));
        tFPR->setGeometry(QRect(100, 170, 71, 20));
        lGPR = new QLabel(gRegister);
        lGPR->setObjectName(QString::fromUtf8("lGPR"));
        lGPR->setGeometry(QRect(10, 140, 20, 20));
        tOp = new QLineEdit(gRegister);
        tOp->setObjectName(QString::fromUtf8("tOp"));
        tOp->setEnabled(false);
        tOp->setGeometry(QRect(40, 80, 71, 20));
        cGPR = new QComboBox(gRegister);
        cGPR->setObjectName(QString::fromUtf8("cGPR"));
        cGPR->setGeometry(QRect(40, 140, 51, 22));
        lOp = new QLabel(gRegister);
        lOp->setObjectName(QString::fromUtf8("lOp"));
        lOp->setGeometry(QRect(10, 80, 21, 16));
        tGPR = new QLineEdit(gRegister);
        tGPR->setObjectName(QString::fromUtf8("tGPR"));
        tGPR->setGeometry(QRect(100, 140, 71, 20));
        tImm = new QLineEdit(gRegister);
        tImm->setObjectName(QString::fromUtf8("tImm"));
        tImm->setEnabled(false);
        tImm->setGeometry(QRect(170, 80, 71, 20));
        lImm = new QLabel(gRegister);
        lImm->setObjectName(QString::fromUtf8("lImm"));
        lImm->setGeometry(QRect(130, 80, 31, 16));
        tB = new QLineEdit(gRegister);
        tB->setObjectName(QString::fromUtf8("tB"));
        tB->setEnabled(false);
        tB->setGeometry(QRect(170, 110, 71, 20));
        lB = new QLabel(gRegister);
        lB->setObjectName(QString::fromUtf8("lB"));
        lB->setGeometry(QRect(140, 110, 16, 16));
        tA = new QLineEdit(gRegister);
        tA->setObjectName(QString::fromUtf8("tA"));
        tA->setEnabled(false);
        tA->setGeometry(QRect(40, 110, 71, 20));
        lA = new QLabel(gRegister);
        lA->setObjectName(QString::fromUtf8("lA"));
        lA->setGeometry(QRect(20, 110, 16, 16));
        tALU = new QLineEdit(gRegister);
        tALU->setObjectName(QString::fromUtf8("tALU"));
        tALU->setEnabled(false);
        tALU->setGeometry(QRect(40, 200, 71, 20));
        lALU = new QLabel(gRegister);
        lALU->setObjectName(QString::fromUtf8("lALU"));
        lALU->setGeometry(QRect(10, 190, 21, 31));
        bSaveReg = new QPushButton(gRegister);
        bSaveReg->setObjectName(QString::fromUtf8("bSaveReg"));
        bSaveReg->setGeometry(QRect(180, 140, 61, 23));
        bStep = new QPushButton(gRegister);
        bStep->setObjectName(QString::fromUtf8("bStep"));
        bStep->setGeometry(QRect(190, 20, 51, 23));
        gResult = new QGroupBox(centralWidget);
        gResult->setObjectName(QString::fromUtf8("gResult"));
        gResult->setGeometry(QRect(790, 10, 321, 231));
        tResults = new QTextEdit(gResult);
        tResults->setObjectName(QString::fromUtf8("tResults"));
        tResults->setGeometry(QRect(10, 20, 301, 201));
        gName = new QGroupBox(centralWidget);
        gName->setObjectName(QString::fromUtf8("gName"));
        gName->setGeometry(QRect(1130, 20, 120, 81));
        line_4 = new QFrame(gName);
        line_4->setObjectName(QString::fromUtf8("line_4"));
        line_4->setGeometry(QRect(80, 20, 61, 61));
        line_4->setFrameShape(QFrame::VLine);
        line_4->setFrameShadow(QFrame::Sunken);
        lName = new QLabel(gName);
        lName->setObjectName(QString::fromUtf8("lName"));
        lName->setGeometry(QRect(10, 20, 91, 61));
        line = new QFrame(gName);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(10, 10, 101, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line_2 = new QFrame(gName);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(10, 70, 101, 20));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        line_3 = new QFrame(gName);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setGeometry(QRect(10, 20, 3, 61));
        line_3->setFrameShape(QFrame::VLine);
        line_3->setFrameShadow(QFrame::Sunken);
        MIPSWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MIPSWindowClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1259, 22));
        MIPSWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MIPSWindowClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MIPSWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MIPSWindowClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MIPSWindowClass->setStatusBar(statusBar);

        retranslateUi(MIPSWindowClass);

        QMetaObject::connectSlotsByName(MIPSWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MIPSWindowClass)
    {
        MIPSWindowClass->setWindowTitle(QApplication::translate("MIPSWindowClass", "MIPS Simulator", 0, QApplication::UnicodeUTF8));
        gCompilerLoader->setTitle(QApplication::translate("MIPSWindowClass", "Compilation + Load Unit", 0, QApplication::UnicodeUTF8));
        bLoadSrc->setText(QApplication::translate("MIPSWindowClass", "&Open Source", 0, QApplication::UnicodeUTF8));
        bAssemble->setText(QApplication::translate("MIPSWindowClass", "&Assemble + Load", 0, QApplication::UnicodeUTF8));
        bLoadConfig->setText(QApplication::translate("MIPSWindowClass", "&Load Config", 0, QApplication::UnicodeUTF8));
        lLoadAt->setText(QApplication::translate("MIPSWindowClass", "Load at", 0, QApplication::UnicodeUTF8));
        tLoad->setText(QApplication::translate("MIPSWindowClass", "00000000", 0, QApplication::UnicodeUTF8));
        gMemory->setTitle(QApplication::translate("MIPSWindowClass", "Memory", 0, QApplication::UnicodeUTF8));
        lStartLoc->setText(QApplication::translate("MIPSWindowClass", "Start at", 0, QApplication::UnicodeUTF8));
        tLoc->setText(QApplication::translate("MIPSWindowClass", "00000000", 0, QApplication::UnicodeUTF8));
        bShow->setText(QApplication::translate("MIPSWindowClass", "&Show", 0, QApplication::UnicodeUTF8));
        bSave_2->setText(QApplication::translate("MIPSWindowClass", "S&ave", 0, QApplication::UnicodeUTF8));
        gRegister->setTitle(QApplication::translate("MIPSWindowClass", "Registers", 0, QApplication::UnicodeUTF8));
        tPC->setText(QApplication::translate("MIPSWindowClass", "00000000", 0, QApplication::UnicodeUTF8));
        lFPR->setText(QApplication::translate("MIPSWindowClass", "FPR", 0, QApplication::UnicodeUTF8));
        bExec->setText(QApplication::translate("MIPSWindowClass", "&Execute", 0, QApplication::UnicodeUTF8));
        lLMD->setText(QApplication::translate("MIPSWindowClass", "LMD", 0, QApplication::UnicodeUTF8));
        lIR->setText(QApplication::translate("MIPSWindowClass", "IR", 0, QApplication::UnicodeUTF8));
        lPC->setText(QApplication::translate("MIPSWindowClass", "PC", 0, QApplication::UnicodeUTF8));
        lGPR->setText(QApplication::translate("MIPSWindowClass", "GPR", 0, QApplication::UnicodeUTF8));
        lOp->setText(QApplication::translate("MIPSWindowClass", "Op", 0, QApplication::UnicodeUTF8));
        lImm->setText(QApplication::translate("MIPSWindowClass", "Imm", 0, QApplication::UnicodeUTF8));
        lB->setText(QApplication::translate("MIPSWindowClass", "B", 0, QApplication::UnicodeUTF8));
        lA->setText(QApplication::translate("MIPSWindowClass", "A", 0, QApplication::UnicodeUTF8));
        lALU->setText(QApplication::translate("MIPSWindowClass", "ALU", 0, QApplication::UnicodeUTF8));
        bSaveReg->setText(QApplication::translate("MIPSWindowClass", "Sa&ve", 0, QApplication::UnicodeUTF8));
        bStep->setText(QApplication::translate("MIPSWindowClass", "&Step", 0, QApplication::UnicodeUTF8));
        gResult->setTitle(QApplication::translate("MIPSWindowClass", "Results", 0, QApplication::UnicodeUTF8));
        gName->setTitle(QString());
        lName->setText(QApplication::translate("MIPSWindowClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Sandipan Dey</span></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">CSEE Department</span></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">611 - Project</span></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; ma"
                        "rgin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">UMBC</span></p>\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(MIPSWindowClass);
    } // retranslateUi

};

namespace Ui {
    class MIPSWindowClass: public Ui_MIPSWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MIPSWINDOW_H
