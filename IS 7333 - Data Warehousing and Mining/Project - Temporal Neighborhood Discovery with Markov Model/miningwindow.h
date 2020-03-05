/* // ----------------------------------------------------------------
   //                   MiningWindow Class
   // ----------------------------------------------------------------
   // Contains the UI Handler Class
   // ----------------------------------------------------------------
   //                   Sandipan Dey
   //                   Graduate Student
   //                   UMBC
   // ----------------------------------------------------------------*/

#ifndef MININGWINDOW_H
#define MININGWINDOW_H

#include <QtGui/QMainWindow>
#include <QThread>
#include <QImage>
#include <QString>
#include <map>
#include "algo.h"
#include "data.h"
#include "plot.h"

namespace Ui
{
    class MainWindowClass;
    class MergeThread;
}

enum GraphicsOperation
{
    NONE,
    PLOT_DATA,
    PLOT_BINS,
    PLOT_MATRIX,
    DRAW_IMAGE,
    DRAW_TREE,
    DRAW_GRAPH,
    DRAW_ALPH_SAX,
    DRAW_DYN_PROG
};

class MergeThread : public QThread
{
    Q_OBJECT

    MiningWindow *window;

 public:

    MergeThread();
    MergeThread(MiningWindow*);
    void run();

 protected slots:

    void MergeFinished();
}; 

class MiningWindow : public QMainWindow
{
    Q_OBJECT

public:
    MiningWindow(QWidget *parent = 0);
    ~MiningWindow();
    void EnableAll(bool = true);

protected:
    virtual void paintEvent ( QPaintEvent * event );

private:
    Ui::MainWindowClass *ui;
    GraphicsOperation currentOperation;

    Algo algo;
    Plotter plotter;
    TemporalData<double> temporalData;
    std::vector<TemporalData<double> > bins;
    QString dir;
    int numData;
    int numBins;
    Distribution distribution;
    DistanceMetric distanceMetric;
    NormalizationMetric normalizationMetric;
    double thresholdLim;
    int windowSize;
    int minBinNumber;
    int dataIndex, binIndex;
    Algorithm algorithm;
    QImage image;
    std::vector<double> steadyStateTransitionVector;
    MergeThread mergeThread;
    bool mergeImage;
    node *tree;
    std::vector<std::list<int> > vg;
    std::vector<char*> alph;
    std::vector<int> px;
    std::vector<int> py;
    std::vector<int> tour;

    void printTSP(std::vector< std::vector<int> >&, int, int, int);

private slots:
    void on_bPartitionIntoEFBins_clicked();
    void on_bGenNLoadData_clicked();
    void on_bMergeBins_clicked();
    void on_bLoadData_clicked();
    void on_bDumpData_clicked();
    void on_bTestThreshold_clicked();
    void on_bLoadImage_clicked();
    void on_bPlotMatrix_clicked();
    void on_bFourier_clicked();
    void on_bCosine_clicked();
    void on_bRandomize_clicked();
    void on_bPlotLeft_clicked();
    void on_bPlotRight_clicked();
    void on_bLoadTimesData_clicked();
    void on_bSegmentImage_clicked();
    void on_bDrawTree_clicked();
    void on_bGenOutlier_clicked();
    void on_bDrawGraph_clicked();
    void on_bSAX_clicked();
    void on_bSAXMerge_clicked();
    void on_bDynProg_clicked();
    void on_cAlgo_currentIndexChanged(int);
    void on_cDistanceMetric_currentIndexChanged(int);
    void on_tBinSize_textEdited(const QString&);
    friend class MergeThread;
    friend class Plotter;
};

#endif // MININGWINDOW_H
