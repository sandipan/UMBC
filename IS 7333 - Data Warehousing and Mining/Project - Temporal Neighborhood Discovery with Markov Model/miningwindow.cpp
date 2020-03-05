/*! // ----------------------------------------------------------------
   //                   MiningWindow Class
   // ----------------------------------------------------------------
   // Contains the UI Handler Class
   // ----------------------------------------------------------------
   //                   Sandipan Dey
   //                   Graduate Student
   //                   UMBC
   // ----------------------------------------------------------------*/

#include <QMessageBox>
#include <QFileDialog>
#include <fstream>
#include <string>
#include <ctime>
#include "MiningWindow.h"
#include "ui_MiningWindow.h"

#define MIN_DATA 30
#define MAX_DATA 20000000
#define MAX_BUF 256

const int DefaultNumData = 1000;
const int DefaultNumBins = 5;
const double DefaultThresholdLim = 1.0;

const char *DistributionString[] =
{
    "None",
    "Gaussian"
};

const char *AlgorithmString[] =
{
     "Similarity with Markov Transition Matrix",     
     "Similarity with Markov Stationary",
     "DynamicProgramming",
     "Greedy"
     /*, "Similarity with DFT" */
};

const char *DistanceMetricString[] =
{
    "Bhattacharya",
    "KL",
    "Mahalanobis",
    "Hellinger"
    /*, "KLSym" */
};

const char *FilterString[] =
{
    "Low Pass",
    "High Pass"
};

const char *DefaultThresholdString[] =
{
    "1.0",
    "0.1",
    "0.8",
    "0.7"
};

const int DefaultWindowSize[] =
{
    5,
    25,
    5,
    5
};

MiningWindow::MiningWindow(QWidget *parent)
    : QMainWindow(parent, Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint), ui(new Ui::MainWindowClass),
      temporalData(0),
      currentOperation(NONE), dir(QString::null),
      numData(DefaultNumData), numBins(DefaultNumBins),
      dataIndex(0), binIndex(-1),
      distribution(None), distanceMetric(Bhattacharya), normalizationMetric(SSD),
      thresholdLim(DefaultThresholdLim),
      mergeThread(this),
      mergeImage(false),
      vg(50)
{
    //srand(time(NULL));
    ui->setupUi(this);

    for (int i = 0; i < (int) sizeof(DistributionString) / (int) sizeof(const char*); ++i)
    {
        ui->cDistribution->addItem(DistributionString[i]);
    }
    for (int i = 0; i < (int) sizeof(AlgorithmString) / (int) sizeof(const char*); ++i)
    {
        ui->cAlgo->addItem(AlgorithmString[i]);
    }
    for (int i = 0; i < (int) sizeof(DistanceMetricString) / (int) sizeof(const char*); ++i)
    {
        ui->cDistanceMetric->addItem(DistanceMetricString[i]);
    }
    for (int i = 0; i < (int) sizeof(FilterString) / (int) sizeof(const char*); ++i)
    {
        ui->cFilter->addItem(FilterString[i]);
    }    
    algo.SetSeed(time(NULL));
    connect(&mergeThread, SIGNAL(finished()), &mergeThread, SLOT(MergeFinished()));
    statusBar()->showMessage(tr("Ready"));
    // ui->bTestThreshold->setVisible(false);

    //std::list<int> l;
    //l.push_back(
    //vg[0] = l;
}

/*std::list<int> adjacent()
{
}*/

MiningWindow::~MiningWindow()
{
    delete ui;
}

/*!
    Generate Random Data
*/
void MiningWindow::on_bGenNLoadData_clicked()
{
    numData = ui->tNumData->text().toInt();
    distribution = (Distribution)ui->cDistribution->currentIndex();
    if ((numData < MIN_DATA) || (numData > MAX_DATA))
    {
         QMessageBox::warning(this, tr("Number of data point not accepted"),
                    QString("Number of data points to be generated must be within ")
                    .append(QString::number(MIN_DATA))
                    .append(QString(" and "))
                    .append(QString::number(MAX_DATA)),
                    QMessageBox::Ok,
                    QMessageBox::Ok);
         return;
    }

    dataIndex = 0;
    temporalData = algo.GenerateRandomData(numData, distribution);
    temporalData = algo.Normalize(temporalData);

    ui->bFourier->setEnabled(true);
    ui->bCosine->setEnabled(true);

    ui->bPlotLeft->setEnabled(numData > MAX_WIDTH);
    ui->bPlotRight->setEnabled(numData > MAX_WIDTH);

    currentOperation = PLOT_DATA;
    update();
}

/*!
    Change data distribution
*/
void MiningWindow::on_bRandomize_clicked()
{
    algo.SetSeed(time(NULL));
}

/*!
    Partion into equal frequency bins
*/
void MiningWindow::on_bPartitionIntoEFBins_clicked()
{
    if (!temporalData.GetSize())
    {
        QMessageBox::warning(this, tr("Error while paritioning data into bins"),
                                tr("No data present.\n"
                                   "Generate data first"),
                                QMessageBox::Ok,
                                QMessageBox::Ok);
        return;
    }

    if ((numBins = ui->tBinSize->text().toInt()) <= 1)
    {
         QMessageBox::warning(this, tr("Invalid number of bins"),
                    tr("Bin number must be greater than 1"),
                    QMessageBox::Ok,
                    QMessageBox::Ok);
         return;
    }

    dataIndex = binIndex = 0;
    bins = algo.PartitionInBins(temporalData, numBins);
    ui->bMergeBins->setEnabled(true);

    ui->bPlotLeft->setEnabled(numData > MAX_WIDTH);
    ui->bPlotRight->setEnabled(numData > MAX_WIDTH);

    currentOperation = PLOT_BINS;
    update();
}

MergeThread::MergeThread()
{
}

MergeThread::MergeThread(MiningWindow *w)
{
    window = w;
}

/*!
    Thread to run merging algorithms
*/
void MergeThread::run()
{
    window->bins = window->algo.RunMergeStates(window->bins, window->algorithm, window->thresholdLim, window->minBinNumber, window->windowSize, AVG, window->distanceMetric, SSD, QString(window->dir + "\\AlgoLog.txt").toLatin1().data(), true, window->mergeImage, window->mergeImage ? window->image.width() : -1, window->mergeImage ? window->image.height() : -1);
    window->tree = window->algo.tr;

    /*std::ofstream outFile1("c:\\test4.txt");

    outFile1 << "Bin size: " << window->bins.size() << std::endl;

    int n = 100;
    int merged = n - window->bins.size();
    int correct = 0, wrong = 0;
    int expected = n - 5;
    int j = 0;

    int left[] = {0, 10, 30, 50, 70};
    int right[] = {9, 29, 49, 69, 99};

    for (int i = 0; i < window->bins.size(); ++i)
    {
        int binSize = window->bins[i].GetIds().size();
        std::vector<int> ids = window->bins[i].GetIds();
        if (ids.size())
        {
            if (ids[0] >= left[j] && ids[binSize - 1] <= right[j])
            {
                if (ids[binSize - 1] == right[j])
                {
                    ++j;
                }
            }
            else if (ids[0] >= left[j] && ids[binSize - 1] > right[j])
            {
                ++j;
                if (ids[0] < left[j])
                {
                    wrong += left[j] - ids[0];
                }
                if (ids[binSize - 1] > right[j])
                {
                    wrong += ids[binSize - 1] - right[j];
                }
            }
            outFile1 << "wrong: " << left[j] << " " << right[j] << "  " << wrong << std::endl;
        }
        outFile1 << "Size of bin " << i << ": " << binSize << std::endl;
    }
    correct = n - window->bins.size() - wrong;

    outFile1 << window->bins.size() << "," << (correct / (merged * 1.0)) * 100 << "," << (correct / (expected * 1.0)) * 100 << ","; */

    if (window->mergeImage == true && !window->image.isNull())
    {
        int x, y, w, h, n;
        x = y = 0;

        w = window->image.width();
        h = window->image.height();
        n = window->bins.size();

        int binDepth = (w * h) / window->numBins;

        for (int i = 0; i < w; ++i)
        {
            for (int j = 0; j < h; ++j)
            {
                window->image.setPixel(i, j, 255);
            }
        }

        for (int i = 0; i < n; ++i)
        {
            x = i % w;
            if (!x)
            {
                y = (y + 1) % h;
            }
            window->image.setPixel(x, y, window->algo.steadyStateTransitionVector[i] * 255);
        }

        /*for (int i = 6; i < 7; ++i)
        {            
            std::vector<int> ids = window->bins[i].GetIds();
            for (int j = 0; j < ids.size(); ++j)
            {                
                x = binDepth * ids[j] % w;
                y = binDepth * ids[j] % h;
                for (int k = 0; k < binDepth; ++k)
                {
                    window->image.setPixel(x, y, 0); //i * 6);
                    x = (x + 1) % w;
                    if (!x)
                    {
                        y = (y + 1) % h;
                    }
                }
            }
        }*/
    }
}

/*!
    Callback to handle the postprocessing
*/
void MergeThread::MergeFinished()
{
    QMessageBox::warning(window, tr("Merging Complete"),
                    QString("Merging Complete"),
                    QMessageBox::Ok,
                    QMessageBox::Ok);

    window->statusBar()->showMessage(tr("Ready"));
    window->steadyStateTransitionVector =  window->algo.steadyStateTransitionVector;
    int n = window->algo.steadyStateTransitionVector.size();
    window->ui->tBinSize->setText(QString::number(window->bins.size()));
    window->EnableAll();
    window->ui->bPlotMatrix->setEnabled(window->algorithm == SimilarityWithMarkovStationary);
    window->ui->bDrawTree->setEnabled(window->algorithm == DynamicProgramming);
    window->ui->bMergeBins->setEnabled(false);

    if (!window->mergeImage)
    {
        window->ui->bPlotLeft->setEnabled(window->numData > MAX_WIDTH);
        window->ui->bPlotRight->setEnabled(window->numData > MAX_WIDTH);
        window->currentOperation = PLOT_BINS;
    }
    else
    {
        window->currentOperation = DRAW_IMAGE;
        window->mergeImage = false;
    }

    window->update();
}

/*!
    Enable/disable buttons
*/
void MiningWindow::EnableAll(bool enableStatus)
{
    ui->bPartitionIntoEFBins->setEnabled(enableStatus);
    ui->bGenNLoadData->setEnabled(enableStatus);
    ui->bMergeBins->setEnabled(enableStatus);
    ui->bLoadData->setEnabled(enableStatus);
    ui->bDumpData->setEnabled(enableStatus);
    //ui->bTestThreshold->setEnabled(enableStatus);
    ui->bLoadImage->setEnabled(enableStatus);
    ui->bPlotMatrix->setEnabled(enableStatus);
    ui->bDrawTree->setEnabled(enableStatus);
    ui->bFourier->setEnabled(enableStatus);
    ui->bCosine->setEnabled(enableStatus);
    ui->bRandomize->setEnabled(enableStatus);
    ui->bPlotLeft->setEnabled(enableStatus);
    ui->bPlotRight->setEnabled(enableStatus);
    ui->bLoadTimesData->setEnabled(enableStatus);
    // ui->bSegmentImage->setEnabled(enableStatus);
}

void MiningWindow::on_cAlgo_currentIndexChanged(int selectedIndex)
{
    ui->tThreshold->setText(DefaultThresholdString[selectedIndex]);
    thresholdLim = ui->tThreshold->text().toDouble();
}

void MiningWindow::on_cDistanceMetric_currentIndexChanged(int selectedIndex)
{
    windowSize = DefaultWindowSize[selectedIndex];
    int n = bins.size();
    if (n && windowSize > n)
    {
        windowSize = n;
    }
    ui->tSlidingWindowSize->setText(QString::number(windowSize));
}

void MiningWindow::on_tBinSize_textEdited(const QString& text)
{
    if (windowSize > numBins)
    {
        windowSize = numBins;
    }
    else
    {
        int selectedIndex = ui->cDistanceMetric->currentIndex();
        windowSize = DefaultWindowSize[selectedIndex];
    }
    ui->tSlidingWindowSize->setText(QString::number(windowSize));
}

void MiningWindow::on_bMergeBins_clicked()
{
    int n;
    if ((n = bins.size()) <= 1)
    {
        QMessageBox::warning(this, tr("Error while merging bins"),
                                tr("At least two bins must be present.\n"
                                   "Create bins first"),
                                QMessageBox::Ok,
                                QMessageBox::Ok);
        return;
    }

    thresholdLim = ui->tThreshold->text().toDouble();
    windowSize = ui->tSlidingWindowSize->text().toInt();
    minBinNumber = ui->tMinBinNumber->text().toInt();
    algorithm = (Algorithm)ui->cAlgo->currentIndex();

    if (algorithm == SimilarityWithTrMatrix && (thresholdLim < 0.1 || thresholdLim > 5))
    {
         QMessageBox::warning(this, tr("Invalid threshold"),
                    QString::fromLatin1("Threshold should be in between 0.1 and 5.0",
                                       strlen("Threshold should be in between 0.1 and 5.0")),
                    QMessageBox::Ok,
                    QMessageBox::Ok);
         return;
    }
    else if (algorithm != SimilarityWithTrMatrix && (thresholdLim <= 0 || thresholdLim >= 1))
    {
         QMessageBox::warning(this, tr("Invalid threshold"),
                    QString::fromLatin1("Threshold should be in between 0 and 1",
                                       strlen("Threshold should be in between 0 and 1")),
                    QMessageBox::Ok,
                    QMessageBox::Ok);
         return;
    }

    if (windowSize <= 0 || windowSize > n)
    {
         QMessageBox::warning(this, tr("Invalid Window Size"),
                    QString::fromLatin1("Window Size should be a +ve integer less than Bin Size",
                                       strlen("Window Size should be a +ve integer less than Bin Size")),
                    QMessageBox::Ok,
                    QMessageBox::Ok);
         return;
    }

    if (windowSize < n && windowSize % 2 == 0)
    {
        ++windowSize;
    }

    if (dir == QString::null)
    {
        dir = QFileDialog::getExistingDirectory(this, tr("Store algorithm logs in:"),
                                                 "C:",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    }

    distanceMetric = (DistanceMetric) ui->cDistanceMetric->currentIndex();

    dataIndex = binIndex = 0;

    // bins = algo.RunMergeStates(bins, algorithm, thresholdLim, minBinNumber, windowSize, AVG, distanceMetric, SSD, QString(dir + "\\AlgoLog.txt").toLatin1().data());

    EnableAll(false);
    mergeThread.start();
    mergeThread.setPriority(QThread::HighestPriority);
    statusBar()->showMessage(tr("Running"));
}

void MiningWindow::on_bPlotMatrix_clicked()
{
    currentOperation = PLOT_MATRIX;
    update();
}

void MiningWindow::on_bPlotLeft_clicked()
{
    if (currentOperation == PLOT_DATA)
    {
        dataIndex -= MAX_WIDTH;
        if (dataIndex < 0)
        {
            dataIndex = 0;
            ui->bPlotLeft->setEnabled(false);
            return;
        }
    }
    else if (currentOperation == PLOT_BINS)
    {
        --binIndex;
        if (binIndex < 0)
        {
            binIndex = 0;
            ui->bPlotLeft->setEnabled(false);
            return;
        }
    }

    ui->bPlotRight->setEnabled(true);    
    update();
}

void MiningWindow::on_bPlotRight_clicked()
{
    if (currentOperation == PLOT_DATA)
    {
        dataIndex += MAX_WIDTH;
        if (dataIndex >= numData)
        {
             dataIndex -= MAX_WIDTH;
             ui->bPlotRight->setEnabled(false);
             return;
        }
    }
    else if (currentOperation == PLOT_BINS)
    {
        ++binIndex;
        if (binIndex >= numBins)
        {
             --binIndex;
             ui->bPlotRight->setEnabled(false);
             return;
        }
    }

    ui->bPlotLeft->setEnabled(true);
    update();
}

void MiningWindow::on_bTestThreshold_clicked()
{
    algo.DoExperiment();
    /*
    if ((numBins = ui->tNumBins->text().toInt()) <= 1)
    {
         QMessageBox::warning(this, tr("Invalid number of bins"),
                    tr("Bin number must be greater than 1"),
                    QMessageBox::Ok,
                    QMessageBox::Ok);
         return;
    }
    for (double t = 1.0 / numBins - EPSILON; t < 1.0 / numBins + EPSILON; t += EPSILON)
    {
        temporalData = algo.GenerateRandomData(numData, distribution);
        bins.clear();
        bins = algo.PartitionInBins(temporalData, numBins, binningAlgo);
        algo.RunMergeStates(bins, threshold, AVG, Mean, SSD, QString(dir + "\\AlgoLog.txt").toLatin1().data());
        tmap.insert(std::pair<double, int>(t, bins.size()));
    }
    currentOperation = THRESHOLD_TEST;
    update(); */
}

void MiningWindow::on_bSAX_clicked()
{
    int n;
    if ((n = bins.size()) <= 1)
    {
        QMessageBox::warning(this, tr("Error while merging bins"),
                                tr("At least two bins must be present.\n"
                                   "Create bins first"),
                                QMessageBox::Ok,
                                QMessageBox::Ok);
        return;
    }
    int w = n;
    int a = ui->taSAX->text().toInt() - 1;
    if (w <= 0 || (a < 3) && (a > 10))
    {
        QMessageBox::warning(this, tr("Can't start SAX"),
                                tr("Both w must be positive and a must be in between 3 & 10"),
                                QMessageBox::Ok,
                                QMessageBox::Ok);
        return;
    }

    double gaussianSplit[][8] // betas
    = {{-0.43, -0.67, -0.84, -0.97, -1.07, -1.15, -1.22, -1.28},
        {0.43,      0, -0.25, -0.43, -0.57, -0.67, -0.76, -0.84},
        {0,     0.67,   0.25,     0, -0.18, -0.32, -0.43, -0.52},
        {0,        0,   0.84,  0.43,  0.18,     0, -0.14, -0.25},
        {0,        0,      0,  0.97,  0.57,  0.32,  0.14,     0},
        {0,        0,      0,     0,  1.07,  0.67,  0.43,  0.25},
        {0,        0,      0,     0,     0,  1.15,  0.76,  0.52},
        {0,        0,      0,     0,     0,     0,  1.22,  0.84},
        {0,        0,      0,     0,     0,     0,     0,  1.28}};

    char *alphabets[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"};
    alph.clear();
    for (int i = 0; i < w; ++i)
    {        
        bool done = false;
        for (int j = 0; j < a - 1; ++j)
        {
            if (bins[i].GetStatistic(AVG) < gaussianSplit[j][a - 3])
            {
                alph.push_back(alphabets[j]);
                done = true;
                break;
            }
        }
        if (!done)
        {
               alph.push_back(alphabets[a]);
        }
    }
    currentOperation = DRAW_ALPH_SAX;
    update();
}

void MiningWindow::on_bSAXMerge_clicked()
{
    int n;
    if ((n = bins.size()) <= 1)
    {
        QMessageBox::warning(this, tr("Error while merging bins"),
                                tr("At least two bins must be present.\n"
                                   "Create bins first"),
                                QMessageBox::Ok,
                                QMessageBox::Ok);
        return;
    }
    int w = n;
    int a = ui->taSAX->text().toInt() - 1;
    if (w <= 0 || (a < 3) && (a > 10))
    {
        QMessageBox::warning(this, tr("Can't start SAX"),
                                tr("Both w must be positive and a must be in between 3 & 10"),
                                QMessageBox::Ok,
                                QMessageBox::Ok);
        return;
    }

    std::ofstream outFile("test.txt");
    int i = 0, j = 0;
    int k = 0;
    while (i < w - 1)
    {
        j = i;
        while ((j < w - 1) && (alph[k] == alph[k + 1]))
        {
            ++j;
            ++k;
        }
        while (i < j)
        {
            algo.MergeBins(bins, i, i + 1, outFile);
            --j;
        }
        w = bins.size();
        ++i;
        ++k;
    }
    outFile.close();
    double gaussianSplit[][8] // betas
    = {{-0.43, -0.67, -0.84, -0.97, -1.07, -1.15, -1.22, -1.28},
        {0.43,      0, -0.25, -0.43, -0.57, -0.67, -0.76, -0.84},
        {0,     0.67,   0.25,     0, -0.18, -0.32, -0.43, -0.52},
        {0,        0,   0.84,  0.43,  0.18,     0, -0.14, -0.25},
        {0,        0,      0,  0.97,  0.57,  0.32,  0.14,     0},
        {0,        0,      0,     0,  1.07,  0.67,  0.43,  0.25},
        {0,        0,      0,     0,     0,  1.15,  0.76,  0.52},
        {0,        0,      0,     0,     0,     0,  1.22,  0.84},
        {0,        0,      0,     0,     0,     0,     0,  1.28}};

    char *alphabets[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"};
    alph.clear();
    for (int i = 0; i < w; ++i)
    {
        bool done = false;
        for (int j = 0; j < a - 1; ++j)
        {
            if (bins[i].GetStatistic(AVG) < gaussianSplit[j][a - 3])
            {
                alph.push_back(alphabets[j]);
                done = true;
                break;
            }
        }
        if (!done)
        {
               alph.push_back(alphabets[a]);
        }
    }
    ui->tBinSize->setText(QString::number(bins.size()));
    currentOperation = DRAW_ALPH_SAX;
    update();
}

double dist(int * px, int * py, int i, int j)
{
    return sqrt((px[i] - px[j]) * (px[i] - px[j]) + (py[i] - py[j]) * (py[i] - py[j]));
}

double dist(std::vector<int> px, std::vector<int> py, int i, int j)
{
    return sqrt((px[i] - px[j]) * (px[i] - px[j]) + (py[i] - py[j]) * (py[i] - py[j]));
}

void MiningWindow::printTSP(std::vector< std::vector<int> >& path, int i, int j, int n)
{
    int k;
    if (n <= 0)
    {
            return;
    }
    if (i <= j)
    {
            k = path[i][j];
            printf("%d", k);
            tour.push_back(k);
            printTSP(path, k, j, --n);
    }
    else
    {
            k = path[j][i];
            printTSP(path, i, k, --n);
            printf("%d", k);
            tour.push_back(k);
    }
}

void MiningWindow::on_bDynProg_clicked()
{
    /*int px[] = {1, 2, 3, 6, 7, 8, 9};
    int py[] = {9, 3, 6, 7, 4, 8, 5};
    int n = sizeof(px) / sizeof(int);
    this->px.clear();
    this->py.clear();
    for (int i = 0; i < n; ++i)
    {
        this->px.push_back(px[i]);
        this->py.push_back(py[i]);
    }*/

    px.clear();
    py.clear();
    int n = 20;
    for (int i = 0; i < n; ++i)
    {
        px.push_back(rand() % 10); // i   // rand() % 10 
        py.push_back(rand() % 10);
    }

    std::vector< std::vector<double> > table(n, std::vector<double>(n));
    std::vector< std::vector<int> > path(n, std::vector<int>(n));
    table[n - 2][n - 1] = dist(px, py, n - 1, n - 2);
    path[n - 2][n - 1] = n - 1;
    for (int i = n - 3; i >= 0; --i)
    {
        double min = 9999;
        int mink = -1;
        for (int k = i + 2; k < n; ++k)
        {
                        if (min > table[i + 1][k] + dist(px, py, i, k))
            {
                min = table[i + 1][k] + dist(px, py, i, k);
                mink = k;
            }
        }
        table[i][i + 1] = min;
        path[i][i + 1] = mink;
        for (int j = i + 2; j < n; ++j)
        {
            table[i][j] = table[i + 1][j] + dist(px, py, i, i + 1);
            path[i][j] = i + 1;
        }
    }
    table[0][0] = table[0][1] + dist(px, py, 0, 1);
    path[0][0] = 1;

    printf("\n");
    printf ("%8d", 0);
    for (int j = 0; j < n; ++j)
    {
            printf ("%8d", j);
    }
    printf("\n");
    for (int i = 0; i < n; ++i)
    {
            printf ("%8d", i);
            for (int j = 0; j < n; ++j)
            {
                    printf ("%8.2f", table[i][j]);
            }
            printf("\n");
    }
    printf("\n");
    printf ("%8d", 0);
    for (int j = 0; j < n; ++j)
    {
            printf ("%8d", j);
    }
    printf("\n");
    for (int i = 0; i < n; ++i)
    {
            printf ("%8d", i);
            for (int j = 0; j < n; ++j)
            {
                    printf ("%8d", path[i][j]);
            }
            printf("\n");
    }
    printf("\n0");
    tour.clear();
    tour.push_back(0);
    printTSP(path, 0, 0, n);
    tour.push_back(0);
    printf("0\n");

    currentOperation = DRAW_DYN_PROG;
    update();
}

void MiningWindow::on_bLoadImage_clicked()
{
    QString imageFile;

    if ((imageFile = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("Images (*.bmp)"))) != QString::null)
    {
        image.load(imageFile);
        currentOperation = DRAW_IMAGE;
        update();
    }
}

void MiningWindow::on_bSegmentImage_clicked()
{
    if (image.isNull())
    {
       QMessageBox::warning(this, tr("No image loaded"),
                     QString::fromLatin1("Load an image first",
                                       strlen("Load an image first")),
                    QMessageBox::Ok,
                    QMessageBox::Ok);
       return;
    }

    std::vector<double> timeSeriesData;

    for (int y = 0; y < image.height(); ++y)
    {
        for (int x = 0; x < image.width(); ++x)
        {            
            timeSeriesData.push_back(qGray(image.pixel(x, y)));
        }
    }

    numData = timeSeriesData.size();

    Q_ASSERT(numData == image.width() * image.height());

    if ((numBins = ui->tBinSize->text().toInt()) <= 1)
    {
         QMessageBox::warning(this, tr("Invalid number of bins"),
                    tr("Bin number must be greater than 1"),
                    QMessageBox::Ok,
                    QMessageBox::Ok);
         return;
    }

    temporalData.SetData(0, timeSeriesData);
    bins = algo.PartitionInBins(temporalData, numBins);

    Q_ASSERT(bins.size() == numBins);

    mergeImage = true;
    on_bMergeBins_clicked();
}

/*!
    Draw the Optimal partition Tree (dynamic programming)
*/
void MiningWindow::on_bDrawTree_clicked()
{
    if (tree)
    {
        currentOperation = DRAW_TREE;
        update();
    }
}

void MiningWindow::on_bGenOutlier_clicked()
{
    dataIndex = 0;
    temporalData = algo.GenerateRandomDataWithOutliers(numData, distribution);
    temporalData = algo.Normalize(temporalData);

    ui->bFourier->setEnabled(true);
    ui->bCosine->setEnabled(true);

    currentOperation = PLOT_DATA;
    update();
}

void MiningWindow::on_bFourier_clicked()
{
    int numCoeff = ui->tNumCoeff->text().toInt();
    if (numCoeff <= 0)
    {
        numCoeff = QMessageBox::warning(this, tr("Invalid number of coefficients"),
                                              tr("Number of Coefficients must be greater than 0"),
                                              QMessageBox::Ok,
                                              QMessageBox::Ok);
         return;
     }     
     algo.DoFourierAndInverseFourierTransform(temporalData, numCoeff, ui->cFilter->currentText() == "Low Pass");
     ui->bFourier->setEnabled(false);
     Q_ASSERT(temporalData.GetSize() == numData);
     currentOperation = PLOT_DATA;
     update();
}

void MiningWindow::on_bCosine_clicked()
{
     int numCoeff = ui->tNumCoeff->text().toInt();
     if (numCoeff <= 0)
     {
        numCoeff = QMessageBox::warning(this, tr("Invalid number of coefficients"),
                                              tr("Number of Coefficients must be greater than 0"),
                                              QMessageBox::Ok,
                                              QMessageBox::Ok);
         return;
     }
     algo.DoCosineAndInverseCosineTransform(temporalData, numCoeff, ui->cFilter->currentText() == "Low Pass");
     ui->bCosine->setEnabled(false);
     Q_ASSERT(temporalData.GetSize() == numData);
     currentOperation = PLOT_DATA;
     update();
}

/*!
    Load data
*/
void MiningWindow::on_bLoadData_clicked()
{
    QString dataFile;
    char buf[MAX_BUF];

    if ((dataFile = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("Data (*.csv *.txt)"))) != QString::null)
    {
        std::ifstream file(dataFile.toLatin1().data());
        std::vector<double> timeSeriesData;
        while (file)
        {
            file.getline(buf, MAX_DATA);
            char *pch = strtok (buf," ,-");
            while (pch != NULL)
            {
                timeSeriesData.push_back(atof(pch));
                pch = strtok (NULL, " ,-");
            }
        }

        dataIndex = 0;
        temporalData.SetData(0, timeSeriesData);
        temporalData = algo.Normalize(temporalData);
        numData = timeSeriesData.size();
        ui->tNumData->setText(QString::number(numData));

        ui->tBinSize->setText("100");

        ui->bPlotLeft->setEnabled(numData > MAX_WIDTH);
        ui->bPlotRight->setEnabled(numData > MAX_WIDTH);

        currentOperation = PLOT_DATA;
        update();
    }
}

/*!
    Load time series data
*/
void MiningWindow::on_bLoadTimesData_clicked()
{
    QString dataFile;
    char buf[MAX_BUF];

    if ((dataFile = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("Data (*.tim)"))) != QString::null)
    {
        std::ifstream file(dataFile.toLatin1().data());
        std::vector<std::string> timeSeriesTime;
        std::vector<double> timeSeriesData;
        while (file)
        {
            file.getline(buf, MAX_DATA);
            int i = 0;
            char *pch = strtok (buf,",");
            while (pch != NULL)
            {
                if (!i)
                {
                    timeSeriesTime.push_back(pch);
                }
                else
                {
                    timeSeriesData.push_back(atof(pch));
                }
                pch = strtok (NULL, ",");
                ++i;
            }
        }

        dataIndex = 0;
        temporalData.SetTime(timeSeriesTime);
        temporalData.SetData(0, timeSeriesData);
        temporalData = algo.Normalize(temporalData);
        numData = timeSeriesData.size();
        ui->tNumData->setText(QString::number(numData));
        ui->tBinSize->setText("100");

        ui->bPlotLeft->setEnabled(numData > MAX_WIDTH);
        ui->bPlotRight->setEnabled(numData > MAX_WIDTH);

        currentOperation = PLOT_DATA;
        update();
    }
}

void MiningWindow::on_bDumpData_clicked()
{
    if (dir == QString::null)
    {
        dir = QFileDialog::getExistingDirectory(this, tr("Select Path to Save (Dump) Data:"),
                                                 "C:",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    }

    if (dir == QString::null)
    {
        QMessageBox::warning(this, tr("Data not saved"),
                            QString::fromLatin1("Data not saved", strlen("Data not saved")),
                            QMessageBox::Ok,
                            QMessageBox::Ok);
        return;
    }

    QString path(dir);
    switch (currentOperation)
    {
        case PLOT_DATA:
            path += "\\data.csv";
            temporalData.Serialize(path.toLatin1().data(), CSV);
            break;

        case PLOT_BINS:
            path += "\\bin";
            for (int i = 0; i < (int) bins.size(); ++i)
            {
               bins[i].Serialize(QString(path + QString::number(i+1).append(".csv")).toLatin1().data(), CSV);
            }

            break;

        default:
            break;
    }

    QMessageBox::warning(this, tr("Saved data"),
                        QString::fromLatin1("Saved in path: ", strlen("Saved in path: ")).append(dir),
                        QMessageBox::Ok,
                        QMessageBox::Ok);
}

void MiningWindow::on_bDrawGraph_clicked()
{
   currentOperation = DRAW_GRAPH;
   update();
}

void MiningWindow::paintEvent ( QPaintEvent * event )
{
    if (currentOperation == PLOT_DATA)
    {
        plotter.Plot(this, temporalData, dataIndex, false, true);
    }
    else if (currentOperation == PLOT_BINS)
    {
        plotter.Plot(this, bins, binIndex);
    }
    else if (currentOperation == PLOT_MATRIX)
    {
        plotter.Plot(this, steadyStateTransitionVector);
    }
    else if (currentOperation == DRAW_IMAGE)
    {
        plotter.Plot(this, image);
    }
    else if (currentOperation == DRAW_TREE)
    {
        plotter.Plot(this, tree);
    }
    else if (currentOperation == DRAW_GRAPH)
    {
        plotter.PlotGraph(this);
    }
    else if (currentOperation == DRAW_ALPH_SAX)
    {
        plotter.Plot(this, bins, binIndex, true, alph);
    }
    else if (currentOperation == DRAW_DYN_PROG)
    {
        plotter.PlotDyn(this);
    }
}
