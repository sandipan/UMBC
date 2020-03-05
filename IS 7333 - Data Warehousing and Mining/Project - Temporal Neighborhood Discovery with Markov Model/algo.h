/* // ----------------------------------------------------------------
   //                   Algortihm Class
   // ----------------------------------------------------------------
   // Contains the algorithms for binning and merging of Markov States
   // ----------------------------------------------------------------
   //                   Sandipan Dey
   //                   Graduate Student
   //                   UMBC
   // ----------------------------------------------------------------*/

#ifndef ALGO_H
#define ALGO_H

#include <vector>
#include "types.h"

#define EPSILON 0.0001 // 0.002 0.005
#define ERROR 0.0001 //0.0000001
#define THRESHOLD(k) (k / (n - 1.0))
#define MAX_THRESHOLD THRESHOLD(1.0)

template <class T>
class TemporalData;
class ofstream;

struct node
{
    int start, end;
    double score;
    node *left, *right;
};

/*!
    Algo class
    Implements all the algorithms
*/
class Algo
{
    public:

    std::vector<double> steadyStateTransitionVector;
    node *tr;
    int seed, dataSet;

    Algo();

    TemporalData<double> GenerateRandomData(int, Distribution);
    TemporalData<double> GenerateRandomDataWithOutliers(int, Distribution);
    std::vector<TemporalData<double> > PartitionInBins(const TemporalData<double>&, int);

    std::vector< std::vector<double> > ComputeTransitionTable(const std::vector<TemporalData<double> >&, Statistic, DistanceMetric, NormalizationMetric, int);
    bool FindStatesToMerge(std::vector< std::vector<double> >&, int, int&, int&, double, std::ofstream&, bool = true);
    bool FindStatesToMerge(std::vector< std::vector<double> >&, int, int&, int&, double, std::ofstream&, bool = true, bool = false, int = -1, int = -1);
    void MergeStates(std::vector< std::vector<double> >&, std::vector<bool>&, int, int, int, std::ofstream&, bool = true);
    std::vector<TemporalData<double> >& MergeBins(std::vector<TemporalData<double> >&, int, int, std::ofstream&, bool = true);
    std::vector<TemporalData<double> >& RunMergeStates(std::vector<TemporalData<double> >&, Algorithm, double, int, int, Statistic, DistanceMetric, NormalizationMetric, const char*, bool = true, bool = false, int = -1, int = -1);
    void NormalizeTransitionTable(std::vector <std::vector<double> >&, int);
    void NormalizeRows(std::vector <std::vector<double> >&, int, int);
    TemporalData<double>& DoFourierAndInverseFourierTransform(TemporalData<double>&, int, bool);
    std::vector<int>& DoFourierAndInverseFourierTransform(std::vector<int>&, int, bool);    
    std::vector<double>& DoFourierAndInverseFourierTransform(std::vector<double>&, int, bool);
    TemporalData<double>& DoCosineAndInverseCosineTransform(TemporalData<double>&, int, bool);
    std::vector<int>& DoCosineAndInverseCosineTransform(std::vector<int>&, int, bool);
    std::vector<double>& DoCosineAndInverseCosineTransform(std::vector<double>&, int, bool);
    double ComputeMetric(int, int, const std::vector<double>&, const std::vector<double>&, DistanceMetric, NormalizationMetric, int);
    double ComputeThreshold(const std::vector< std::vector<double> >&, int);
    double MaxRowSum(const std::vector< std::vector<double> >&, int);
    std::vector< std::vector<double> > ComputeSteadyStateTransition(const std::vector< std::vector<double> >&, int, std::ofstream&, bool = true);
    std::vector< std::vector<double> > Multiply(const std::vector< std::vector<double> >&, const std::vector< std::vector<double> >&, int);
    double Error(const std::vector< std::vector<double> >&, int);
    void DumpTransitionTable(std::ofstream&, const std::vector<std::vector<double> >&, std::vector<bool>&) const;
    TemporalData<double>& Normalize(TemporalData<double>&, bool = true);

    void SetSeed(int);
    void DoExperiment();
};

#endif // ALGO_H
