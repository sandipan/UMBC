/********************************************************************************
** Form generated from reading UI file 'miningwindow.ui'
**
** Created: Sat Jun 26 09:01:29 2010
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MININGWINDOW_H
#define UI_MININGWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindowClass
{
public:
    QWidget *centralWidget;
    QGroupBox *gGenData;
    QPushButton *bGenNLoadData;
    QLabel *lNumData;
    QLineEdit *tNumData;
    QComboBox *cDistribution;
    QLabel *lDistribution;
    QPushButton *bRandomize;
    QGroupBox *gMergeBins;
    QPushButton *bMergeBins;
    QLabel *lThreshold;
    QComboBox *cDistanceMetric;
    QLineEdit *tThreshold;
    QLineEdit *tSlidingWindowSize;
    QLabel *lSlidingWindowSize;
    QLabel *lMinBin;
    QLineEdit *tMinBinNumber;
    QPushButton *bPlotMatrix;
    QLabel *lDistanceMetric;
    QComboBox *cAlgo;
    QLabel *lAlgo;
    QPushButton *bDrawTree;
    QPushButton *bSAX;
    QLineEdit *taSAX;
    QLabel *laSAX;
    QPushButton *bSAXMerge;
    QGroupBox *gImageProcess;
    QPushButton *bLoadImage;
    QPushButton *bSegmentImage;
    QPushButton *bDrawGraph;
    QPushButton *bDynProg;
    QGroupBox *gDataLeftRight;
    QPushButton *bPlotLeft;
    QPushButton *bPlotRight;
    QGroupBox *gPartitionIntoBins;
    QPushButton *bPartitionIntoEFBins;
    QLineEdit *tBinSize;
    QLabel *lBinSize;
    QGroupBox *gTesting;
    QPushButton *bLoadData;
    QPushButton *bLoadTimesData;
    QGroupBox *gFourier;
    QPushButton *bFourier;
    QLabel *lNumCoeff;
    QLineEdit *tNumCoeff;
    QLabel *lFilter;
    QComboBox *cFilter;
    QPushButton *bCosine;
    QGroupBox *gTest;
    QPushButton *bTestThreshold;
    QPushButton *bDumpData;
    QPushButton *bGenOutlier;
    QGroupBox *gInfo;
    QLabel *lName;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindowClass)
    {
        if (MainWindowClass->objectName().isEmpty())
            MainWindowClass->setObjectName(QString::fromUtf8("MainWindowClass"));
        MainWindowClass->resize(1035, 726);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindowClass->sizePolicy().hasHeightForWidth());
        MainWindowClass->setSizePolicy(sizePolicy);
        MainWindowClass->setMinimumSize(QSize(1035, 725));
        centralWidget = new QWidget(MainWindowClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gGenData = new QGroupBox(centralWidget);
        gGenData->setObjectName(QString::fromUtf8("gGenData"));
        gGenData->setGeometry(QRect(20, 460, 191, 141));
        gGenData->setAutoFillBackground(false);
        bGenNLoadData = new QPushButton(gGenData);
        bGenNLoadData->setObjectName(QString::fromUtf8("bGenNLoadData"));
        bGenNLoadData->setGeometry(QRect(10, 80, 171, 23));
        lNumData = new QLabel(gGenData);
        lNumData->setObjectName(QString::fromUtf8("lNumData"));
        lNumData->setGeometry(QRect(10, 20, 81, 20));
        tNumData = new QLineEdit(gGenData);
        tNumData->setObjectName(QString::fromUtf8("tNumData"));
        tNumData->setGeometry(QRect(100, 20, 81, 20));
        cDistribution = new QComboBox(gGenData);
        cDistribution->setObjectName(QString::fromUtf8("cDistribution"));
        cDistribution->setEnabled(true);
        cDistribution->setGeometry(QRect(100, 50, 81, 22));
        cDistribution->setAutoFillBackground(false);
        lDistribution = new QLabel(gGenData);
        lDistribution->setObjectName(QString::fromUtf8("lDistribution"));
        lDistribution->setGeometry(QRect(10, 50, 81, 20));
        bRandomize = new QPushButton(gGenData);
        bRandomize->setObjectName(QString::fromUtf8("bRandomize"));
        bRandomize->setGeometry(QRect(10, 110, 171, 23));
        gMergeBins = new QGroupBox(centralWidget);
        gMergeBins->setObjectName(QString::fromUtf8("gMergeBins"));
        gMergeBins->setGeometry(QRect(220, 520, 391, 171));
        bMergeBins = new QPushButton(gMergeBins);
        bMergeBins->setObjectName(QString::fromUtf8("bMergeBins"));
        bMergeBins->setGeometry(QRect(230, 110, 151, 23));
        lThreshold = new QLabel(gMergeBins);
        lThreshold->setObjectName(QString::fromUtf8("lThreshold"));
        lThreshold->setGeometry(QRect(230, 50, 101, 20));
        cDistanceMetric = new QComboBox(gMergeBins);
        cDistanceMetric->setObjectName(QString::fromUtf8("cDistanceMetric"));
        cDistanceMetric->setEnabled(true);
        cDistanceMetric->setGeometry(QRect(130, 50, 81, 20));
        tThreshold = new QLineEdit(gMergeBins);
        tThreshold->setObjectName(QString::fromUtf8("tThreshold"));
        tThreshold->setEnabled(true);
        tThreshold->setGeometry(QRect(340, 50, 41, 20));
        tSlidingWindowSize = new QLineEdit(gMergeBins);
        tSlidingWindowSize->setObjectName(QString::fromUtf8("tSlidingWindowSize"));
        tSlidingWindowSize->setEnabled(true);
        tSlidingWindowSize->setGeometry(QRect(130, 80, 81, 20));
        lSlidingWindowSize = new QLabel(gMergeBins);
        lSlidingWindowSize->setObjectName(QString::fromUtf8("lSlidingWindowSize"));
        lSlidingWindowSize->setGeometry(QRect(10, 80, 111, 20));
        lMinBin = new QLabel(gMergeBins);
        lMinBin->setObjectName(QString::fromUtf8("lMinBin"));
        lMinBin->setGeometry(QRect(10, 110, 111, 20));
        tMinBinNumber = new QLineEdit(gMergeBins);
        tMinBinNumber->setObjectName(QString::fromUtf8("tMinBinNumber"));
        tMinBinNumber->setEnabled(true);
        tMinBinNumber->setGeometry(QRect(130, 110, 81, 20));
        bPlotMatrix = new QPushButton(gMergeBins);
        bPlotMatrix->setObjectName(QString::fromUtf8("bPlotMatrix"));
        bPlotMatrix->setEnabled(false);
        bPlotMatrix->setGeometry(QRect(10, 140, 201, 23));
        lDistanceMetric = new QLabel(gMergeBins);
        lDistanceMetric->setObjectName(QString::fromUtf8("lDistanceMetric"));
        lDistanceMetric->setGeometry(QRect(10, 50, 111, 20));
        cAlgo = new QComboBox(gMergeBins);
        cAlgo->setObjectName(QString::fromUtf8("cAlgo"));
        cAlgo->setEnabled(true);
        cAlgo->setGeometry(QRect(160, 20, 221, 22));
        cAlgo->setAutoFillBackground(false);
        lAlgo = new QLabel(gMergeBins);
        lAlgo->setObjectName(QString::fromUtf8("lAlgo"));
        lAlgo->setGeometry(QRect(10, 20, 141, 20));
        bDrawTree = new QPushButton(gMergeBins);
        bDrawTree->setObjectName(QString::fromUtf8("bDrawTree"));
        bDrawTree->setEnabled(false);
        bDrawTree->setGeometry(QRect(230, 140, 151, 21));
        bSAX = new QPushButton(gMergeBins);
        bSAX->setObjectName(QString::fromUtf8("bSAX"));
        bSAX->setGeometry(QRect(290, 80, 31, 23));
        taSAX = new QLineEdit(gMergeBins);
        taSAX->setObjectName(QString::fromUtf8("taSAX"));
        taSAX->setEnabled(true);
        taSAX->setGeometry(QRect(260, 80, 21, 20));
        laSAX = new QLabel(gMergeBins);
        laSAX->setObjectName(QString::fromUtf8("laSAX"));
        laSAX->setGeometry(QRect(230, 80, 21, 20));
        bSAXMerge = new QPushButton(gMergeBins);
        bSAXMerge->setObjectName(QString::fromUtf8("bSAXMerge"));
        bSAXMerge->setGeometry(QRect(320, 80, 61, 23));
        gImageProcess = new QGroupBox(centralWidget);
        gImageProcess->setObjectName(QString::fromUtf8("gImageProcess"));
        gImageProcess->setGeometry(QRect(830, 460, 191, 151));
        bLoadImage = new QPushButton(gImageProcess);
        bLoadImage->setObjectName(QString::fromUtf8("bLoadImage"));
        bLoadImage->setEnabled(true);
        bLoadImage->setGeometry(QRect(10, 30, 171, 21));
        bSegmentImage = new QPushButton(gImageProcess);
        bSegmentImage->setObjectName(QString::fromUtf8("bSegmentImage"));
        bSegmentImage->setEnabled(false);
        bSegmentImage->setGeometry(QRect(10, 60, 171, 21));
        bDrawGraph = new QPushButton(gImageProcess);
        bDrawGraph->setObjectName(QString::fromUtf8("bDrawGraph"));
        bDrawGraph->setGeometry(QRect(10, 90, 171, 23));
        bDynProg = new QPushButton(gImageProcess);
        bDynProg->setObjectName(QString::fromUtf8("bDynProg"));
        bDynProg->setGeometry(QRect(10, 120, 171, 23));
        gDataLeftRight = new QGroupBox(centralWidget);
        gDataLeftRight->setObjectName(QString::fromUtf8("gDataLeftRight"));
        gDataLeftRight->setGeometry(QRect(20, 430, 1001, 21));
        bPlotLeft = new QPushButton(gDataLeftRight);
        bPlotLeft->setObjectName(QString::fromUtf8("bPlotLeft"));
        bPlotLeft->setEnabled(false);
        bPlotLeft->setGeometry(QRect(0, 0, 31, 23));
        bPlotRight = new QPushButton(gDataLeftRight);
        bPlotRight->setObjectName(QString::fromUtf8("bPlotRight"));
        bPlotRight->setEnabled(false);
        bPlotRight->setGeometry(QRect(970, 0, 31, 23));
        gPartitionIntoBins = new QGroupBox(centralWidget);
        gPartitionIntoBins->setObjectName(QString::fromUtf8("gPartitionIntoBins"));
        gPartitionIntoBins->setGeometry(QRect(220, 460, 391, 51));
        bPartitionIntoEFBins = new QPushButton(gPartitionIntoBins);
        bPartitionIntoEFBins->setObjectName(QString::fromUtf8("bPartitionIntoEFBins"));
        bPartitionIntoEFBins->setGeometry(QRect(200, 20, 181, 23));
        tBinSize = new QLineEdit(gPartitionIntoBins);
        tBinSize->setObjectName(QString::fromUtf8("tBinSize"));
        tBinSize->setGeometry(QRect(100, 20, 91, 20));
        lBinSize = new QLabel(gPartitionIntoBins);
        lBinSize->setObjectName(QString::fromUtf8("lBinSize"));
        lBinSize->setGeometry(QRect(10, 20, 81, 20));
        gTesting = new QGroupBox(centralWidget);
        gTesting->setObjectName(QString::fromUtf8("gTesting"));
        gTesting->setGeometry(QRect(20, 610, 191, 81));
        bLoadData = new QPushButton(gTesting);
        bLoadData->setObjectName(QString::fromUtf8("bLoadData"));
        bLoadData->setGeometry(QRect(10, 20, 171, 21));
        bLoadTimesData = new QPushButton(gTesting);
        bLoadTimesData->setObjectName(QString::fromUtf8("bLoadTimesData"));
        bLoadTimesData->setGeometry(QRect(10, 50, 171, 23));
        gFourier = new QGroupBox(centralWidget);
        gFourier->setObjectName(QString::fromUtf8("gFourier"));
        gFourier->setGeometry(QRect(620, 460, 201, 111));
        bFourier = new QPushButton(gFourier);
        bFourier->setObjectName(QString::fromUtf8("bFourier"));
        bFourier->setEnabled(false);
        bFourier->setGeometry(QRect(10, 80, 91, 21));
        lNumCoeff = new QLabel(gFourier);
        lNumCoeff->setObjectName(QString::fromUtf8("lNumCoeff"));
        lNumCoeff->setGeometry(QRect(10, 20, 111, 20));
        tNumCoeff = new QLineEdit(gFourier);
        tNumCoeff->setObjectName(QString::fromUtf8("tNumCoeff"));
        tNumCoeff->setEnabled(true);
        tNumCoeff->setGeometry(QRect(130, 20, 61, 20));
        lFilter = new QLabel(gFourier);
        lFilter->setObjectName(QString::fromUtf8("lFilter"));
        lFilter->setGeometry(QRect(10, 50, 81, 20));
        cFilter = new QComboBox(gFourier);
        cFilter->setObjectName(QString::fromUtf8("cFilter"));
        cFilter->setEnabled(true);
        cFilter->setGeometry(QRect(100, 50, 91, 20));
        bCosine = new QPushButton(gFourier);
        bCosine->setObjectName(QString::fromUtf8("bCosine"));
        bCosine->setEnabled(false);
        bCosine->setGeometry(QRect(110, 80, 81, 21));
        gTest = new QGroupBox(centralWidget);
        gTest->setObjectName(QString::fromUtf8("gTest"));
        gTest->setGeometry(QRect(620, 580, 201, 111));
        bTestThreshold = new QPushButton(gTest);
        bTestThreshold->setObjectName(QString::fromUtf8("bTestThreshold"));
        bTestThreshold->setEnabled(true);
        bTestThreshold->setGeometry(QRect(10, 80, 181, 21));
        bDumpData = new QPushButton(gTest);
        bDumpData->setObjectName(QString::fromUtf8("bDumpData"));
        bDumpData->setGeometry(QRect(10, 50, 181, 23));
        bGenOutlier = new QPushButton(gTest);
        bGenOutlier->setObjectName(QString::fromUtf8("bGenOutlier"));
        bGenOutlier->setGeometry(QRect(10, 20, 181, 23));
        gInfo = new QGroupBox(centralWidget);
        gInfo->setObjectName(QString::fromUtf8("gInfo"));
        gInfo->setGeometry(QRect(830, 720, 191, 101));
        lName = new QLabel(centralWidget);
        lName->setObjectName(QString::fromUtf8("lName"));
        lName->setGeometry(QRect(830, 620, 191, 71));
        MainWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindowClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1035, 22));
        MainWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindowClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindowClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindowClass->setStatusBar(statusBar);

        retranslateUi(MainWindowClass);

        QMetaObject::connectSlotsByName(MainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowClass)
    {
        MainWindowClass->setWindowTitle(QApplication::translate("MainWindowClass", "Unequal Depth Discretization & Generation of Temporal Neighborhoods", 0, QApplication::UnicodeUTF8));
        MainWindowClass->setStyleSheet(QString());
        gGenData->setStyleSheet(QApplication::translate("MainWindowClass", "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 orange, stop: 1 yellow);\n"
"border: 2px solid brown; border-radius: 5px; \n"
"font: bold;", 0, QApplication::UnicodeUTF8));
        gGenData->setTitle(QApplication::translate("MainWindowClass", "Generate Synthetic Data", 0, QApplication::UnicodeUTF8));
        bGenNLoadData->setStyleSheet(QString());
        bGenNLoadData->setText(QApplication::translate("MainWindowClass", "Randomly &Generate Data", 0, QApplication::UnicodeUTF8));
        lNumData->setStyleSheet(QString());
        lNumData->setText(QApplication::translate("MainWindowClass", "Data Size", 0, QApplication::UnicodeUTF8));
        tNumData->setStyleSheet(QApplication::translate("MainWindowClass", "background: white", 0, QApplication::UnicodeUTF8));
        tNumData->setText(QApplication::translate("MainWindowClass", "1000", 0, QApplication::UnicodeUTF8));
        cDistribution->setStyleSheet(QString());
        lDistribution->setText(QApplication::translate("MainWindowClass", "Distribution", 0, QApplication::UnicodeUTF8));
        bRandomize->setStyleSheet(QString());
        bRandomize->setText(QApplication::translate("MainWindowClass", "Change Random &Seed", 0, QApplication::UnicodeUTF8));
        gMergeBins->setStyleSheet(QApplication::translate("MainWindowClass", "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0  white, stop: 1 skyblue);\n"
"border: 2px solid darkblue; border-radius: 5px; \n"
"font: bold;\n"
"", 0, QApplication::UnicodeUTF8));
        gMergeBins->setTitle(QApplication::translate("MainWindowClass", "Merge Bins", 0, QApplication::UnicodeUTF8));
        bMergeBins->setStyleSheet(QString());
        bMergeBins->setText(QApplication::translate("MainWindowClass", "&Merge Bins", 0, QApplication::UnicodeUTF8));
        lThreshold->setText(QApplication::translate("MainWindowClass", "Threshold Frac", 0, QApplication::UnicodeUTF8));
        tThreshold->setStyleSheet(QApplication::translate("MainWindowClass", "background: white", 0, QApplication::UnicodeUTF8));
        tThreshold->setText(QApplication::translate("MainWindowClass", "1.0", 0, QApplication::UnicodeUTF8));
        tSlidingWindowSize->setStyleSheet(QApplication::translate("MainWindowClass", "background: white", 0, QApplication::UnicodeUTF8));
        tSlidingWindowSize->setText(QApplication::translate("MainWindowClass", "5", 0, QApplication::UnicodeUTF8));
        lSlidingWindowSize->setText(QApplication::translate("MainWindowClass", "Window Size", 0, QApplication::UnicodeUTF8));
        lMinBin->setText(QApplication::translate("MainWindowClass", "Minimum Bin Size", 0, QApplication::UnicodeUTF8));
        tMinBinNumber->setStyleSheet(QApplication::translate("MainWindowClass", "background: white", 0, QApplication::UnicodeUTF8));
        tMinBinNumber->setText(QApplication::translate("MainWindowClass", "2", 0, QApplication::UnicodeUTF8));
        bPlotMatrix->setStyleSheet(QString());
        bPlotMatrix->setText(QApplication::translate("MainWindowClass", "Plot &Steady State Vector", 0, QApplication::UnicodeUTF8));
        lDistanceMetric->setText(QApplication::translate("MainWindowClass", "Distance Measure", 0, QApplication::UnicodeUTF8));
        lAlgo->setText(QApplication::translate("MainWindowClass", "Algorithm to be used", 0, QApplication::UnicodeUTF8));
        bDrawTree->setStyleSheet(QString());
        bDrawTree->setText(QApplication::translate("MainWindowClass", "Draw &Tree", 0, QApplication::UnicodeUTF8));
        bSAX->setStyleSheet(QString());
        bSAX->setText(QApplication::translate("MainWindowClass", "SA&X", 0, QApplication::UnicodeUTF8));
        taSAX->setStyleSheet(QApplication::translate("MainWindowClass", "background: white", 0, QApplication::UnicodeUTF8));
        taSAX->setText(QApplication::translate("MainWindowClass", "5", 0, QApplication::UnicodeUTF8));
        laSAX->setText(QApplication::translate("MainWindowClass", "a", 0, QApplication::UnicodeUTF8));
        bSAXMerge->setStyleSheet(QString());
        bSAXMerge->setText(QApplication::translate("MainWindowClass", "SAXMerg", 0, QApplication::UnicodeUTF8));
        gImageProcess->setStyleSheet(QApplication::translate("MainWindowClass", "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 lightgreen, stop: 1 yellow);\n"
"border: 2px solid green; border-radius: 5px; \n"
"font: bold;\n"
"", 0, QApplication::UnicodeUTF8));
        gImageProcess->setTitle(QApplication::translate("MainWindowClass", "Image Processing", 0, QApplication::UnicodeUTF8));
        bLoadImage->setStyleSheet(QString());
        bLoadImage->setText(QApplication::translate("MainWindowClass", "Load &Image", 0, QApplication::UnicodeUTF8));
        bSegmentImage->setStyleSheet(QString());
        bSegmentImage->setText(QApplication::translate("MainWindowClass", "&SegmentImage", 0, QApplication::UnicodeUTF8));
        bDrawGraph->setText(QApplication::translate("MainWindowClass", "Draw &Graph", 0, QApplication::UnicodeUTF8));
        bDynProg->setText(QApplication::translate("MainWindowClass", "&DynProg", 0, QApplication::UnicodeUTF8));
        gDataLeftRight->setStyleSheet(QApplication::translate("MainWindowClass", "background: qlineargradient(x1: 0, y1: 0, x2: 2, y2: 1, stop: 0 white, stop: 1 gray);\n"
"border: 2px solid black; border-radius: 5px; \n"
"font: bold;", 0, QApplication::UnicodeUTF8));
        gDataLeftRight->setTitle(QString());
        bPlotLeft->setText(QApplication::translate("MainWindowClass", "<<", 0, QApplication::UnicodeUTF8));
        bPlotRight->setText(QApplication::translate("MainWindowClass", ">>", 0, QApplication::UnicodeUTF8));
        gPartitionIntoBins->setStyleSheet(QApplication::translate("MainWindowClass", "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 skyblue, stop: 1 lightgreen);\n"
"border: 2px solid black; border-radius: 5px; \n"
"font: bold;", 0, QApplication::UnicodeUTF8));
        gPartitionIntoBins->setTitle(QApplication::translate("MainWindowClass", "Partition into bins", 0, QApplication::UnicodeUTF8));
        bPartitionIntoEFBins->setStyleSheet(QString());
        bPartitionIntoEFBins->setText(QApplication::translate("MainWindowClass", "&Partition into Bins", 0, QApplication::UnicodeUTF8));
        tBinSize->setStyleSheet(QApplication::translate("MainWindowClass", "background: white", 0, QApplication::UnicodeUTF8));
        tBinSize->setText(QApplication::translate("MainWindowClass", "10", 0, QApplication::UnicodeUTF8));
        lBinSize->setText(QApplication::translate("MainWindowClass", "Bin Size", 0, QApplication::UnicodeUTF8));
        gTesting->setStyleSheet(QApplication::translate("MainWindowClass", "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 pink, stop: 1 orange);\n"
"border: 2px solid red; border-radius: 5px; \n"
"font: bold;\n"
"", 0, QApplication::UnicodeUTF8));
        gTesting->setTitle(QApplication::translate("MainWindowClass", "Load Data (From File)", 0, QApplication::UnicodeUTF8));
        bLoadData->setStyleSheet(QString());
        bLoadData->setText(QApplication::translate("MainWindowClass", "&Load Data (XLS)", 0, QApplication::UnicodeUTF8));
        bLoadTimesData->setStyleSheet(QString());
        bLoadTimesData->setText(QApplication::translate("MainWindowClass", "Load &Time Series Data ", 0, QApplication::UnicodeUTF8));
        gFourier->setStyleSheet(QApplication::translate("MainWindowClass", "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 silver, stop: 1 pink);\n"
"border: 2px solid magenta; border-radius: 5px; \n"
"font: bold;\n"
"", 0, QApplication::UnicodeUTF8));
        gFourier->setTitle(QApplication::translate("MainWindowClass", "Fourier Transform", 0, QApplication::UnicodeUTF8));
        bFourier->setStyleSheet(QString());
        bFourier->setText(QApplication::translate("MainWindowClass", "D&FT + IDFT", 0, QApplication::UnicodeUTF8));
        lNumCoeff->setText(QApplication::translate("MainWindowClass", "# of Coefficients", 0, QApplication::UnicodeUTF8));
        tNumCoeff->setStyleSheet(QApplication::translate("MainWindowClass", "background: white", 0, QApplication::UnicodeUTF8));
        tNumCoeff->setText(QApplication::translate("MainWindowClass", "10", 0, QApplication::UnicodeUTF8));
        lFilter->setText(QApplication::translate("MainWindowClass", "Filter Type", 0, QApplication::UnicodeUTF8));
        bCosine->setStyleSheet(QString());
        bCosine->setText(QApplication::translate("MainWindowClass", "D&CT+IDCT", 0, QApplication::UnicodeUTF8));
        gTest->setStyleSheet(QApplication::translate("MainWindowClass", "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 white, stop: 1 silver);\n"
"border: 2px solid black; border-radius: 5px; \n"
"font: bold;\n"
"", 0, QApplication::UnicodeUTF8));
        gTest->setTitle(QApplication::translate("MainWindowClass", "Testing", 0, QApplication::UnicodeUTF8));
        bTestThreshold->setStyleSheet(QString());
        bTestThreshold->setText(QApplication::translate("MainWindowClass", "&Test ", 0, QApplication::UnicodeUTF8));
        bDumpData->setStyleSheet(QString());
        bDumpData->setText(QApplication::translate("MainWindowClass", "&Dump Data (To File)", 0, QApplication::UnicodeUTF8));
        bGenOutlier->setStyleSheet(QString());
        bGenOutlier->setText(QApplication::translate("MainWindowClass", "Generate Data with &Outliers", 0, QApplication::UnicodeUTF8));
        gInfo->setStyleSheet(QString());
        gInfo->setTitle(QString());
        lName->setStyleSheet(QApplication::translate("MainWindowClass", "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 yellow, stop: 1 skyblue);\n"
"border: 2px solid darkgreen; border-radius: 5px; font: bold;", 0, QApplication::UnicodeUTF8));
        lName->setText(QApplication::translate("MainWindowClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt; font-weight:600;\">Sandipan Dey</span></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt; font-weight:600;\">Graduate Student</span></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt; font-weight:600;\">Information Systems</span></p>\n"
"<p align=\"center\" style"
                        "=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt; font-weight:600;\">UMBC</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindowClass: public Ui_MainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MININGWINDOW_H
