/* // ----------------------------------------------------------------
   //                   Plotter Class
   // ----------------------------------------------------------------
   // Contains the Class that plots data
   // ----------------------------------------------------------------
   //                   Sandipan Dey
   //                   Graduate Student
   //                   UMBC
   // ----------------------------------------------------------------*/

#ifndef PLOT_H
#define PLOT_H

#include <vector>
#include <QImage>

#define X_START 20
#define Y_START 20
#define Y_END 420
#define MAX_HEIGHT 400.0
#define MAX_WIDTH 1000.0
#define TREE_ROOT_X 500
#define TREE_ROOT_Y 25
#define TREE_NODE_GAP 250

#define UNINITIALIZED -1.0

class MiningWindow;
template <class T>
class TemporalData;
struct node;

class Plotter
{
    double* data;

    public:
    Plotter();
    ~Plotter();
    void Plot(MiningWindow*, TemporalData<double>&, int = 0, bool = true, bool = false, char* = NULL, double = UNINITIALIZED, double = UNINITIALIZED, int = 2);
    void Plot(MiningWindow*, std::vector<TemporalData<double> >&, int, bool = false, std::vector<char*> = std::vector<char*>(NULL));
    void Plot(MiningWindow*, std::vector<double>&);
    void Plot(MiningWindow*, QImage&);
    void Plot(MiningWindow*, node*);
    void PlotGraph(MiningWindow*);
    void PlotDyn(MiningWindow*);
    void PlotThreshold(MiningWindow*, double ampThreshold, int finalBinSize);
    void Clear(MiningWindow* miningWindow);
    double* getData();
};

#endif // PLOT_H
