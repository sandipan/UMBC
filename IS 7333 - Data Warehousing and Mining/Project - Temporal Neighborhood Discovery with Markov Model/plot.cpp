/* // ----------------------------------------------------------------
   //                   Plotter Class
   // ----------------------------------------------------------------
   // Contains the Class that plots data
   // ----------------------------------------------------------------
   //                   Sandipan Dey
   //                   Graduate Student
   //                   UMBC
   // ----------------------------------------------------------------*/

#include <QPainter>
#include "miningwindow.h"
#include "data.h"
#include "plot.h"

void plot(QPainter& p, double x,double y,double z);

Plotter::Plotter()
{
     //data = new double[500 * 500 * 3];
     //data = getData();
}

Plotter::~Plotter()
{
    //delete [] data;
}

void Plotter::Plot(MiningWindow *miningWindow, TemporalData<double>& data, int startTime, bool shiftData, bool cleasrScr, char* alphabet, double max, double min, int color)
{
     QPainter painter(miningWindow);
     //painter.setPen(Qt::blue);

     int xC = X_START, yC = Y_END, yU = Y_START;
     double maxH = MAX_HEIGHT; // maxW = 500.0;

     if (cleasrScr)
     {
       painter.fillRect(xC, yU, MAX_WIDTH, yC - yU, Qt::white);
     }

     if (max == UNINITIALIZED || min == UNINITIALIZED)
     {
         max = data.GetStatistic(MAX);
         min = data.GetStatistic(MIN);
     }

     double yScale = maxH / (max - min);
     // int xScale = maxW / data.GetSize();

     std::vector<std::string> timeSeriesTime = data.GetTime();
     std::vector<double> timeSeriesData = data.GetData();

     painter.setPen(color % 3 ? color % 3 == 1 ? Qt::yellow : Qt::green : Qt::red);

     std::vector<std::string>::const_iterator itt;
     if (timeSeriesTime.size())
     {
         itt = timeSeriesTime.begin() + (startTime * !shiftData);
     }

     painter.setFont(QFont("Times", 12, QFont::Bold));

     int t = startTime;
     for (std::vector<double>::const_iterator it = timeSeriesData.begin() + (startTime * !shiftData); it != timeSeriesData.end(); ++it, ++t)
     {
        int x = shiftData ? xC + t : xC + t - startTime;
        painter.drawLine(x, yC, x, (int) (yC - (*it - min) * yScale));
        if (timeSeriesTime.size() && itt != timeSeriesTime.end())
        {
            if (t % 200 == 0)
            {
                painter.setPen(Qt::black);
                painter.drawLine(x, yC + 5, x, (yC - 5));
                painter.drawText(x, yC + 20, (*itt).c_str());
                painter.setPen(color % 3 ? color % 3 == 1 ? Qt::yellow : Qt::green : Qt::red);
            }
            ++itt;
        }
     }

     painter.setPen(Qt::blue);
     painter.drawRect(xC, yU, t < MAX_WIDTH ? t : MAX_WIDTH, yC - yU);
     painter.drawLine(xC + startTime, yC, xC + startTime, yU);

     if (alphabet)
     {
          painter.drawText(5 + (xC + startTime + (t < MAX_WIDTH ? t : MAX_WIDTH)) / 2, MAX_HEIGHT / 2, alphabet);
     }
     /*painter.setBrush(Qt::black);

     std::ifstream inFile("C:\\test.txt");
     char c;
     int line = 400, x = 800;
     int count = 1;
     while (inFile)
     {
        c = inFile.get();
        if (c == ' ' || c == '\t' || c == '\n')
        {
            continue;
        }
        if (count > 17)
        {
            line -= 2;
            count = 1;
            x = 800;
        }
        for (int i = 7; i >= 0; --i, x -= 2)
        {
            if ((c >> i) & 0x01)
            {
                painter.drawEllipse(x + 10, 500 - line, 2, 2);
            }
        }
        ++count;
     }*/
}

void Plotter::Plot(MiningWindow* miningWindow, std::vector<TemporalData<double> >& bins, int startBin, bool alphPlot, std::vector<char*> alph)
{
    Q_ASSERT(bins.size() >= 1);

    double max = bins[0].GetStatistic(MAX);
    double min = bins[0].GetStatistic(MIN);

    for (int i = 1; i < (int) bins.size(); ++i)
    {
        double curMax = bins[i].GetStatistic(MAX);
        double curMin = bins[i].GetStatistic(MIN);
        if (max < curMax)
        {
            max = curMax;
        }
        if (min > curMin)
        {
            min = curMin;
        }        
    }

    int startTime = 0;
    int color = startBin % 3;

    for (int i = startBin; i < (int) bins.size(); startTime += bins[i].GetSize(), ++i)
    {
       Plot(miningWindow, bins[i], startTime, true, !startTime, alphPlot? alph[i] : 0, max, min, color);
       //int n = bins[i].GetTime().size();
       color = (color + 1) % 3;
    }
}

void Plotter::Plot(MiningWindow* miningWindow, QImage& image)
{
    QPainter painter(miningWindow);
    int xC = X_START, yC = Y_START;

    painter.drawImage(xC, yC, image);

    /*
    for (int x = 0; x < image.width() * 5; x += 5)
    {
        for (int y = 0; y < image.height() * 5; y += 5)
        {
            painter.drawRect(x + 300, y + 300, 5, 5);
        }
    }

     painter.setBrush(Qt::green);
     if (miningWindow->bins.size())
     {
         for (int i = 6; i < 7; ++i)
         {
            std::vector<int> ids = miningWindow->bins[i].GetIds();
            for (int j = 0; j < ids.size(); ++j)
            {
                int x = ids[j] % image.width();
                int y = ids[j] % image.height();
                painter.drawRect(5 * x + 300, 5 * y + 300, 5, 5);
            }
        }
     }
     */
    //for (int i = 0; i < 500 * 500; i += 3)
    //{
    //     plot(painter, data[i], data[i + 1], data[i + 2]);
    //}
}

const int X0 = 200;
const int Y0 = 200;
const int X0Z = 50;
const int Y0Z = 300;
const int X = 400;
const int Y = 50;
const double L = sqrt((X0Z-X0)*(X0Z-X0)+(Y0Z-Y0)*(Y0Z-Y0));
const double ZCF = 0.5;

double* Plotter::getData()
{
    int i = 0;
    for (double x = -250; x <  250; x += 0.5)
    {
        for (double y = -250; y <  250; y += 0.5)
        {
            data[i++] = x;
            data[i++] = y;
            data[i++] = 100 * sin((x + y) / 5);
        }
    }
    return data;
}

void plot(QPainter& p, double x,double y,double z)
{
        double tx=X0-(y/L)*(X0-X0Z);
        double ty=Y0+y/L*(Y0Z-Y0);
        //g.setColor(new Color(255, 0, 0));
        p.drawEllipse((int)(tx+x), (int)(ty-ZCF*z), 2, 2);
}

void drawTree(QPainter& p, node *n, int x, int y, int g)
{
    static int xC = 10;    
    if (n)
    {
        g /= 1.5;
        p.drawEllipse(xC + x, y, 5, 5);
        if (n->start == n->end)
        {
            p.drawText(xC + x + 2, y + 15, QString::number(n->start));
        }
        if (n->left)
        {
            p.drawLine(xC + x, y, xC + x - g, y + 30);
            drawTree(p, n->left, x - g, y + 30, g);
        }
        if (n->right)
        {
            p.drawLine(xC + x, y, xC + x + g, y + 30);
            drawTree(p, n->right, x + g, y + 30, g);
        }
    }
}

void Plotter::Plot(MiningWindow* miningWindow, node *n)
{
    QPainter painter(miningWindow);
    painter.fillRect(0, 0, miningWindow->width(), miningWindow->height(), Qt::white);
    painter.setBrush(Qt::red);
    drawTree(painter, n, TREE_ROOT_X, TREE_ROOT_Y, TREE_NODE_GAP);
}

void Plotter::Plot(MiningWindow* miningWindow, std::vector<double>& steadyStateTransitionVector)
{
    QPainter painter(miningWindow);
    int xC = X_START, yC = Y_END / 2;

    painter.fillRect(0, 0, miningWindow->width(), miningWindow->height(), Qt::white);
    painter.setBrush(Qt::red);
    painter.setPen(Qt::red);

    for (int i = 0; i < steadyStateTransitionVector.size(); ++i)
    {
        painter.drawEllipse(xC + i * 5, yC - steadyStateTransitionVector[i] * 5000, 2, 2);
        if (i < steadyStateTransitionVector.size() - 1)
        {
            painter.drawLine(xC + i * 5, yC - steadyStateTransitionVector[i] * 5000, xC + (i + 1) * 5, yC - steadyStateTransitionVector[i + 1] * 5000);
        }
    }
}

void Plotter::Clear(MiningWindow* miningWindow)
{
     QPainter painter(miningWindow);
     painter.fillRect(0, 0, miningWindow->width(), miningWindow->height(), Qt::white);   
}

void Plotter::PlotThreshold(MiningWindow* miningWindow, double ampThreshold, int finalBinSize)
{
     QPainter painter(miningWindow);

     int xC = X_START, yC = Y_END, yU = Y_START;

     QPen pen(Qt::red, 15);
     painter.setPen(pen);
     painter.drawRect(xC, yU, xC + yC, yC - yU);

     painter.fillRect(0, 0, miningWindow->width(), miningWindow->height(), Qt::white);
     painter.setPen(Qt::green);

     painter.drawEllipse(xC + (int) ampThreshold, yC - finalBinSize, 2, 2);
}

 int xC = 400, yC = 220;
 int r = 210;
 int n = 50;
 double theta = 2 * M_PI / n;

void join(QPainter& p, int i, int j)
{
    p.drawLine(xC + r * cos(i * theta), yC - r * sin(i * theta), xC + r * cos(j * theta), yC - r * sin(j * theta));
}

void Plotter::PlotGraph(MiningWindow* w)
{
     QPainter painter(w);

     for (int i = 0; i < n; ++i)
     {
        painter.drawEllipse(xC + r * cos(i * theta), yC - r * sin(i * theta), 2, 2);
        painter.drawText(xC + r * cos(i * theta), yC - r * sin(i * theta), QString::number(i));
        join(painter, 40, 11); join(painter, 40, 48);
        join(painter, 48, 46);
        join(painter, 46, 25);
        join(painter, 25, 6);
        join(painter, 6, 14);
        join(painter, 14, 34);
        join(painter, 34, 19);
        join(painter, 19, 37);
        join(painter, 37, 13); join(painter, 37, 33); join(painter, 37, 38);
        join(painter, 13, 0); join(painter, 13, 24); join(painter, 13, 47);
        join(painter, 38, 48); join(painter, 38, 49);
        join(painter, 24, 14);
        join(painter, 41, 32); join(painter, 41, 43);
        join(painter, 31, 4); join(painter, 31, 43);
        join(painter, 42, 9); join(painter, 42, 36);
        join(painter, 32, 29); join(painter, 32, 41);
        join(painter, 21, 29); join(painter, 21, 45);
        join(painter, 39, 8); join(painter, 39, 22);
        join(painter, 18, 5); join(painter, 18, 16); join(painter, 18, 35); join(painter, 18, 44);
     }
}

void Plotter::PlotDyn(MiningWindow* w)
{
    QPainter painter(w);
    xC = yC = 300;

    painter.setPen(Qt::cyan);
    for (int x = 0; x < 600; x += 25)
    {
         for (int y = 0; y < 300; y += 25)
         {
            painter.drawLine(xC + x, yC, xC + x, yC - 300);
            painter.drawLine(xC, yC - y, xC + 600, yC - y);
         }
    }
    painter.setBrush(Qt::darkGreen);
    painter.setPen(Qt::darkGreen);
    for (int i = 0; i < w->px.size(); ++i)
    {
        painter.drawEllipse(xC + 25 * w->px[i] - 2, yC - 25 * w->py[i] - 2, 5, 5);
    }
    painter.setPen(Qt::darkBlue);
    for (int i = 0; i < w->tour.size() - 1; ++i)
    {
        if (w->tour[i] == w->tour[i + 1])
        {
            painter.setPen(Qt::darkRed);
        }
        painter.drawLine(xC + 25 * w->px[w->tour[i]], yC - 25 * w->py[w->tour[i]],
                         xC + 25 * w->px[w->tour[i + 1]], yC - 25 * w->py[w->tour[i + 1]]);
    }
}
