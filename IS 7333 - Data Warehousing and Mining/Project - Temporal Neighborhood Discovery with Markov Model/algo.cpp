/*! // ----------------------------------------------------------------
   //                   Algortihm Class
   // ----------------------------------------------------------------
   // Contains the algorithms for binning and merging of Markov States
   // ----------------------------------------------------------------
   //                   Sandipan Dey
   //                   Graduate Student
   //                   UMBC
   // ----------------------------------------------------------------*/


#include <QtGlobal>
#include <QBitArray>
#include <QTime>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <float.h>
#include <limits>
#include <queue>
#include <ctime>
#include "data.h"
#include "plot.h"
#include "algo.h"

#define TABLE_SPACE 13 //12
#define PROB_PRECS 6 // 18L
#define MAX_STEPS 100 // 100 // 10
#define MAX_DATA 256

/*! Constructor */
Algo::Algo() : dataSet(-1)
{
    //srand(time(NULL));
}

/*! Functor to (uniformly) generate (pseudo) random numbers.
    Can scale the range of random numbers to be generated from (0, RAND_MAX)
    by an input parameter scale > 0 to the constructor.
    It will generate random numbers in the range (0, RAND_MAX / scale) */
class Random
{
    int scale;

    public:
    Random (int sc) : scale(sc)
    {
    }

    int operator() ()
    {
        return (int)(rand() / scale);
    }
};

/*! Functor to (uniformly) generate (pseudo) random numbers, another variant.
    Can generate random numbers in the range (lb, ub), where
    (lb, ub) are parameters to the constructor */
class Random1
{
    int lb, ub;

    public:
    Random1 (int l, int u) : lb(l), ub(u)
    {
    }

    int operator() ()
    {
        return (int)(lb + rand() * (ub - lb) / RAND_MAX);
    }
};

int iseed = time(NULL);

/*! Two Gaussian random numbers generated from two uniform
   random numbers.  Copyright (c) Tao Pang 1997. */
void grnf (double *x, double *y)
{
    double pi,r1,r2;
    double ranf();

    pi =  4*atan(1);
    r1 = -log(1-ranf());
    r2 =  2*pi*ranf();
    r1 =  sqrt(2*r1);
    *x  = r1*cos(r2);
    *y  = r1*sin(r2);
}

/*! (Yet another) Uniform random number generator x(n+1)= a*x(n) mod c
   with a = pow(7,5) and c = pow(2,31)-1.
   Copyright (c) Tao Pang 1997. */
double ranf()
{
    const int ia=16807,ic=2147483647,iq=127773,ir=2836;
    int il,ih,it;
    double rc;

    ih = iseed/iq;
    il = iseed%iq;
    it = ia*il-ir*ih;

    if (it > 0)
    {
        iseed = it;
    }
    else
    {
        iseed = ic+it;
    }

    rc = ic;

    return iseed/rc;
}

/*! Generate Gaussian random numbers using Box-Muller transform */
double gaussian(void)
{
   static double v, fac;
   static int phase = 0;
   double S, Z, U1, U2, u;

   if (phase)
      Z = v * fac;
   else
   {
      do
      {
         U1 = (double)rand() / RAND_MAX;
         U2 = (double)rand() / RAND_MAX;

         u = 2. * U1 - 1.;
         v = 2. * U2 - 1.;
         S = u * u + v * v;
      } while(S >= 1);

      fac = sqrt (-2. * log(S) / S);
      Z = u * fac;
   }

   phase = 1 - phase;

   return Z;
}

const double c_0 = 2.515517;
const double c_1 = 0.802853;
const double c_2 = 0.010328;
const double d_1 = 1.432788;
const double d_2 = 0.189269;
const double d_3 = 0.001308;

/*! Functor to find the next Gaussian (pseudo) random numbers */
class NextGaussian
{
    int mu, sigma;

    public:

    NextGaussian(double dMu, double dSigma) : mu (dMu), sigma(dSigma)
    {
    }

    double CumulativeGaussian(double p)
    {
        // p is a rectangular probability between 0 and 1
        // convert that into a gaussian.
        // Apply the inverse cumulative gaussian distribution function
        // This is an approximation by Abramowitz and Stegun; Press, et al.
        // See http://www.pitt.edu/~wpilib/statfaq/gaussfaq.html
        // Because of the symmetry of the normal
        // distribution, we need only consider 0 < p < 0.5. If you have p > 0.5,
        // then apply the algorithm below to q = 1-p, and then negate the value
        // for X obtained.

        bool fNegate = false;

        if (p > 0.5)
        {
            p = 1.0 - p;
            fNegate = true;
        }

        double t = sqrt(log(1.0 / (p * p)));
        double tt = t * t;
        double ttt = tt * t;
        double X = t - ((c_0 + c_1 * t + c_2 * tt) / (1 + d_1 * t + d_2 * tt + d_3 * ttt));

        return fNegate ? -X : X;
    }

    /// Choose the next random number. The distribution of randomness is "normal"
    /// or "Gaussian" with a mean value dMu and a standard deviation of dSigma
    /// <param name="dMu">The center point (mean) of the normal distribution</param>
    /// <param name="dSigma">The standard deviation of the distribution</param>
    /// <returns>Random value</returns>
    int operator ()()
    {
        double p;
        do
        {
            p = rand() / (1.0 * RAND_MAX);
        }
        while (p > 0.5);
        return mu + CumulativeGaussian(p) * sigma;
    }
};

/*!
    Generates Temporal Data randomly.
    Can generate different random temporal data distributions.
    Inputs - number of tuples (data points) to generate
             distribution to follow (e.g., uniform or Gaussian).
    Returns - temporal data object.
*/
TemporalData<double> Algo::GenerateRandomData(int numberOfTuples, Distribution pdf)
{
    Q_ASSERT(numberOfTuples != 0);
    TemporalData<double> d(0);
    std::vector<double> timeSeriesData, temp;
    char buf[MAX_DATA];

    srand(seed);

    switch(pdf)
    {
        case Gaussian:

             switch (dataSet)
             {
                 default:
                 case 0:

                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 10, NextGaussian(5, 50));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, NextGaussian(1, 20));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 10, NextGaussian(7, 70));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 10, NextGaussian(8, 50));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, NextGaussian(2, 10));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 10, NextGaussian(9, 30));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, NextGaussian(10, 100));

                 break;

                 case 1:

                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 20, NextGaussian(5, 50));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 20, NextGaussian(6, 50));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 20, NextGaussian(1, 20));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 20, NextGaussian(2, 20));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 20, NextGaussian(2, 20));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 20, NextGaussian(1, 20));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 10, NextGaussian(10, 50));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 10, NextGaussian(8, 20));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 10, NextGaussian(1, 50));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 10, NextGaussian(2, 30));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, NextGaussian(10, 100));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 10, NextGaussian(10, 100));

                 break;

             }

             d.SetData(0, timeSeriesData);

             break;


        case None:

        default:

             switch (dataSet)
             {
                 default:
                 case 0:

                 // ---------   Data Set 1  ---------
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 10, Random(50));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random(10));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 10, Random(70));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 10, Random(80));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random(20));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 10, Random(90));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random(100));

                 break;

                 case 1:

                 // ---------   Data Set 2  ---------
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 3, Random(10));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 2, Random(100));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 6 + numberOfTuples % 6, Random(30));

                 break;

                 case 2:

                 // ---------   Data Set 3  ---------
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random(10));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random(100));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random(30));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random(100));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5 + numberOfTuples % 5, Random(10));

                 break;

                 case 3:

                 // ---------   Data Set 4  ---------
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random(10));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random(100));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random(75));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random(10));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5 + numberOfTuples % 5, Random(10));

                 break;

                 case 4:

                 // ---------   Data Set 5  ---------
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random(30));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random(100));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random(100));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random(10));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5 + numberOfTuples % 5, Random(10));

                 break;

                 case 5:

                 /* */
                 // ---------   Data Set 6  ---------
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 10, Random(100));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 10, Random(10));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 10, Random(10));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 10, Random(10));
                 std::generate_n(std::back_inserter(timeSeriesData), 3* numberOfTuples / 20, Random(30));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 20, Random(76));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 10, Random(75));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 10, Random(78));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 10, Random(77));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 10 + numberOfTuples % 10, Random(10));
                 /* */

                 break;

                 /* *\/
                 // ---------   Data Set 7  ---------
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random1(2000, 2005));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random1(1000, 1005));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random1(100, 105));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random1(1000, 1005));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5 + numberOfTuples % 5, Random1(2000, 2005));
                 /\* */

                 /* *\/
                 // ---------   Data Set 8  ---------
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random1(2000, 2005));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random1(2000, 2005));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random1(100, 105));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5, Random1(1000, 1005));
                 std::generate_n(std::back_inserter(timeSeriesData), numberOfTuples / 5 + numberOfTuples % 5, Random1(1000, 1005));
                 /\* */
             }

             d.SetData(0, timeSeriesData);

             break;
    }
    return d;
}

/*!
    Generates random temporal data with outliers
    Inputs - number of tuples (data points) to generate
             distribution to follow (e.g., uniform).
    Returns - temporal data object.
    (Right now it can generate only one fixed noisy temporal pattern with uniform distribution, can be extended)
*/
TemporalData<double> Algo::GenerateRandomDataWithOutliers(int numberOfTuples, Distribution pdf)
{
     Q_ASSERT(numberOfTuples != 0);
     TemporalData<double> d(0);
     std::vector<double> timeSeriesData;

     srand(seed);

     // ---------   Data Set with Outliers  ---------
     std::generate_n(std::back_inserter(timeSeriesData), 25 , Random(50));
     std::generate_n(std::back_inserter(timeSeriesData), 2 , Random(10));
     std::generate_n(std::back_inserter(timeSeriesData), 20 , Random(50));
     std::generate_n(std::back_inserter(timeSeriesData), 1 , Random(10));
     std::generate_n(std::back_inserter(timeSeriesData), 25 , Random(50));
     std::generate_n(std::back_inserter(timeSeriesData), 2 , Random(10));
     std::generate_n(std::back_inserter(timeSeriesData), 25 , Random(50));
     std::generate_n(std::back_inserter(timeSeriesData), 99, Random(10));
     std::generate_n(std::back_inserter(timeSeriesData), 2, Random(90));
     std::generate_n(std::back_inserter(timeSeriesData), 99, Random(10));
     std::generate_n(std::back_inserter(timeSeriesData), 100, Random(70));
     std::generate_n(std::back_inserter(timeSeriesData), 25 , Random(80));
     std::generate_n(std::back_inserter(timeSeriesData), 2 , Random(10));
     std::generate_n(std::back_inserter(timeSeriesData), 20 , Random(80));
     std::generate_n(std::back_inserter(timeSeriesData), 1 , Random(10));
     std::generate_n(std::back_inserter(timeSeriesData), 25 , Random(80));
     std::generate_n(std::back_inserter(timeSeriesData), 2 , Random(10));
     std::generate_n(std::back_inserter(timeSeriesData), 25 , Random(80));
     std::generate_n(std::back_inserter(timeSeriesData), 200, Random(20));
     std::generate_n(std::back_inserter(timeSeriesData), 100, Random(90));
     std::generate_n(std::back_inserter(timeSeriesData), 49, Random(100));
     std::generate_n(std::back_inserter(timeSeriesData), 1 , Random(10));
     std::generate_n(std::back_inserter(timeSeriesData), 49, Random(100));
     std::generate_n(std::back_inserter(timeSeriesData), 1 , Random(10));
     std::generate_n(std::back_inserter(timeSeriesData), 49, Random(100));
     std::generate_n(std::back_inserter(timeSeriesData), 2 , Random(10));
     std::generate_n(std::back_inserter(timeSeriesData), 50, Random(100));

     d.SetData(0, timeSeriesData);

     return d;
}

/*! Normalize the temporal data generated.
    Input - temporal data object
    Returns - normalized data object */
TemporalData<double>& Algo::Normalize(TemporalData<double>& data, bool zScore)
{
    double max = -999999, min = 9999999, sum = 0, sumsq = 0, avg = 0, sd = 0;
    std::vector<double> d = data.GetData();
    int n = d.size();

    for (int i = 0; i < n; ++i)
    {
        if (max < d[i])
        {
            max = d[i];
        }
        else if (min > d[i])
        {
            min = d[i];
        }
        sum += d[i];
        sumsq += d[i] * d[i];
    }

    avg = sum / n;
    sd = sqrt(sumsq / n - avg * avg);

    if (!zScore)
    {
        for (int i = 0; i < n; ++i)
        {
            d[i] = (max != min) ? (d[i] - min)  / (max - min) : 1;
        }
    }
    else
    {
        for (int i = 0; i < n; ++i)
        {
            d[i] = (d[i] - avg) / sd;
        }
    }

    data.SetData(0, d);

    return data;
}

/*!
    Parition temporal data into equal depth bins
    Inputs - temporal data object
             number of bins to divide into
    Returns - bins as collection of temporal data objects.
*/
std::vector<TemporalData<double> > Algo::PartitionInBins(const TemporalData<double>& data, int numOfBins)
{
    std::vector<TemporalData<double> > bins;
    std::vector<double> timeSeriesData = data.GetData();
    std::vector<std::string> timeSeriesTime = data.GetTime();

    Q_ASSERT(numOfBins != 0);
    int dataSize = timeSeriesData.size();
    int binSize = dataSize / numOfBins;

    std::vector<std::string>::const_iterator itt;

    if (timeSeriesTime.size())
    {
        itt = timeSeriesTime.begin();
    }

    int curBin = 1;
    for (std::vector<double>::const_iterator it = timeSeriesData.begin();  it != timeSeriesData.end(); ++curBin)
    {
        if (!timeSeriesTime.size())
        {
            TemporalData<double> bin(curBin - 1, it, curBin == numOfBins ? it + binSize + (dataSize % numOfBins) : it + binSize);
            bins.push_back(bin);
        }
        else
        {            
            TemporalData<double> bin(curBin - 1, it, curBin == numOfBins ? it + binSize + (dataSize % numOfBins) : it + binSize, itt, curBin == numOfBins ? itt + binSize + (dataSize % numOfBins) : itt + binSize);
            bins.push_back(bin);
            itt += ((curBin == numOfBins) ? binSize + (dataSize % numOfBins) : binSize);
        }
        it += ((curBin == numOfBins) ? binSize + (dataSize % numOfBins) : binSize);
    }

    return bins;
}

/*std::vector<TemporalData<double> > Algo::PartitionIn2DBins(const TemporalData<double>& data, int binNumber, BinningAlgo algo)
{
    std::vector<TemporalData<double> > hBins = PartitionInBins(data, binNumber, algo), bins;
    for (int i = 0; i < binNumber; ++i)
    {
        std::vector<TemporalData<double> > vBins = PartitionInBins(hBins[i], binNumber, algo);
        std::copy(bins.begin(), bins.end(), std::back_inserter(vBins));
    }
}*/

/*!
    Computes the transition table.
    Inputs - bins as collection of temporal data objects.
             distance metric (Bhattacharyya, KL, Mahalnonbis etc.)
             window size for a given data point.
    Returns - normalized (row-stochastic) transition table computed.
*/
std::vector <std::vector<double> > Algo::ComputeTransitionTable(const std::vector<TemporalData<double> >& bins, Statistic statistic, DistanceMetric dMetric, NormalizationMetric nMetric, int windowSize)
{
    int n = bins.size();
    std::vector <std::vector<double> > transitionTable(n, std::vector<double>(n));
    std::vector<double> means, sds;

    switch (statistic)
    {
        case AVG:

            for(int i = 0; i < (int)bins.size(); ++i)
            {
                means.push_back(bins[i].GetStatistic(AVG));
                sds.push_back(bins[i].GetStatistic(SD));
            }

            for (int row = 0; row < n; ++row)
            {
                int count = 0;

                int start = 0;

                if (windowSize < n)
                {
                    if (row - (windowSize - 1) / 2 > 0)
                    {
                        start = row - (windowSize - 1) / 2;
                    }
                    if (row + (windowSize - 1) / 2 >= n)
                    {
                        start = n - windowSize;
                    }
                }

                for (int col =  start; count <  (windowSize < n ? windowSize : n); ++col, ++count)
                //for (int col =  row - windowSize / 2 < 0 ? 0 :  row - windowSize / 2; col < n && col <  row + windowSize / 2  + windowSize % 2; ++col)
                //for (int col = 0; col < n; ++col)
                {
                    /*transitionTable[row][col] = 0;
                    if (row == col)
                    {
                        continue;
                    }
                    else if ((row >= col - windowSize / 2) || (row <= col + windowSize / 2 + windowSize % 2))
                    {
                        transitionTable[row][col] = ComputeMetric(row, col, means, sds, dMetric, nMetric, windowSize);
                    }*/
                    if (row == col)
                    {
                        transitionTable[row][col] = 0;
                    }
                    else
                    {
                        transitionTable[row][col] = ComputeMetric(row, col, means, sds, dMetric, nMetric, windowSize);
                    }
                }
            }

            NormalizeRows(transitionTable, n, windowSize);

            break;

        default:

            break;
        }

        return transitionTable;
}

/*!
    Normalizes each row of the transition table (converts the transtion matrix to a row-stochastic matrix).
    Inputs - bins as collection of temporal data objects.
             size of the row (e.g., number of bins).
*/
void Algo::NormalizeRows(std::vector <std::vector<double> >& transitionTable, int n, int windowSize)
{
    for (int row = 0; row < n; ++row)
    {
        double sum = 0;

        int count = 0;

        int start = 0;

        if (windowSize < n)
        {
            if (row - (windowSize - 1) / 2 > 0)
            {
                start = row - (windowSize - 1) / 2;
            }
            if (row + (windowSize - 1) / 2 >= n)
            {
                start = n - windowSize;
            }
        }

        //for (int col =  row - windowSize / 2 < 0 ? 0 :  row - windowSize / 2; col < n && col <  row + windowSize / 2  + windowSize % 2; ++col)
        //for (int col = 0; col < n; ++col)
        for (int col =  start; count <  (windowSize < n ? windowSize : n); ++col, ++count)
        {
            //if ((row >= col - windowSize / 2) || (row <= col + windowSize / 2 + windowSize % 2))
            //{
                sum += transitionTable[row][col];
            //}
        }

        //Q_ASSERT(sum > 0);

        count = 0;
        if (sum)
        {
            //for (int col =  row - windowSize / 2 < 0 ? 0 :  row - windowSize / 2; col < n && col <  row + windowSize / 2  + windowSize % 2; ++col)
            //for (int col = 0; col < n; ++col)
            for (int col =  start; count <  (windowSize < n ? windowSize : n); ++col, ++count)
            {
                //if ((row >= col - windowSize / 2) || (row <= col + windowSize / 2 + windowSize % 2))
                //{
                    transitionTable[row][col] /= sum;
                //}
            }
        }
    }
}

/*!
    Normalizes the transition table (converts the transtion matrix to a doubly-stochastic matrix).
    Inputs - bins as collection of temporal data objects.
             size of the row (e.g., number of bins).
    (Not in use)
*/
void Algo::NormalizeTransitionTable(std::vector <std::vector<double> >& transitionTable, int n)
{
    double sum = 0;
    for (int row = 0; row < n; ++row)
    {
        for (int col = 0; col < n; ++col)
        {
            sum += transitionTable[row][col];
        }
    }

    for (int row = 0; row < n; ++row)
    {
        for (int col = 0; col < n; ++col)
        {
            transitionTable[row][col] /= sum;
        }
    }

    /*
    std::ofstream outFile("d:\\mstoc.txt", std::ofstream::app);
    std::vector<bool> deleted(n, false);

    DumpTransitionTable(outFile, transitionTable, deleted);

    // Richard Sinkhorn: iterative process of alternative normalization of rows and columns

    std::vector <std::vector<double> > newTable(n, std::vector<double>(n));
    double error = 0;

    for (int row = 0; row < n; ++row)
    {
        for (int col = 0; col < n; ++col)
        {
            newTable[row][col] = transitionTable[row][col];
        }
    }

    do
    {
        // must not have any zero entries
        // transitionTable[i][i] = DBL_EPSILON;

        // normalize rows
        for (int row = 0; row < n; ++row)
        {
            double colSum = 0;
            for (int col = 0; col < n; ++col)
            {
                colSum += newTable[row][col];
            }
            for (int col = 0; col < n; ++col)
            {
                newTable[row][col] /=  colSum;
            }
        }

        DumpTransitionTable(outFile, newTable, deleted);

        // normalize columns
        for (int col = 0; col < n; ++col)
        {
            double rowSum = 0;
            for (int row = 0; row < n; ++row)
            {
                rowSum += newTable[row][col];
            }
            for (int row = 0; row < n; ++row)
            {
                newTable[row][col] /= rowSum;
            }
        }

        DumpTransitionTable(outFile, newTable, deleted);

        error = Error(transitionTable, newTable, n);

        for (int row = 0; row < n; ++row)
        {
            for (int col = 0; col < n; ++col)
            {
                newTable[row][col] = transitionTable[row][col];
            }
        }
    }
    while (error > EPSILON);
    */
}

/*!
    Computes the distance (in distribution) between two bins using different distance metrics.
    Inputs - bin indices (i, j)
             temporal summerization (bin means and standard deviations)
             distance metric to be used (e.g., Bhattacharyya, KL, Mahalanobis etc.)
             window size to be used for computation.
    Returns - computed distance.
*/

double Algo::ComputeMetric(int i, int j, const std::vector<double>& means, const std::vector<double>& sds, DistanceMetric dMetric, NormalizationMetric nMetric, int windowSize)
{
        int n = (int) means.size();
        double numerator, denominator;
        double mi = means[i], mj = means[j], si = sds[i], sj = sds[j];

        switch (nMetric)
        {
            case SSD:

                switch (dMetric)
                {
                    /* case Mean:

                        numerator = ((mi - mj) * (mi - mj)); // / ((si * si + sj * sj) ? (si * si + sj * sj) : 1);
                        denominator = 0;
                        int lb = i - windowSize / 2 < 0, ub = i + windowSize / 2 + windowSize % 2;
                        lb = lb < 0 ? 0 : lb;
                        ub = ub >= n ? n - 1 : ub;
                        for (int k = lb; k <= ub; ++k)
                        //for (int k = 0; k < n; ++k)
                        {
                            denominator += ((mi - means[k]) * (mi - means[k])); // / ((si * si + sds[k] * sds[k]) ? (si * si + sds[k] * sds[k]) : 1);
                            // denominator += (mi - means[k]) * (mi - means[k]);
                        }                     

                        return exp(-(numerator / denominator));  // (n > 2) ? (1.0 - (numerator / denominator)) / (n - 2.0) : 0; // 1.0 / (numerator / denominator); //
                    */

                    case Bhattacharya:

                        return  exp(-(0.25 * ((mi - mj) * (mi - mj)) / (si * si + sj * sj)
                                   + 0.5 * ((si && sj) ? log(si / (2.0 * sj) + sj / (2.0 * si)) : 0)));

                    case KL:

                        return exp(-((sj ? 0.5 * (mi - mj) * (mi - mj) / (/*2 * */ sj * sj) : 0)
                                   + (si && sj ? log(sj / si) : 0) + 0.5 * ((sj == 1 ? 0 : (si * si) / (sj * sj)) - 1)));

                    /*
                    case KLSym:

                        return exp(-((sj ? 0.5 * (mi - mj) * (mi - mj) * (1.0 /(si * si) + 1.0 / (sj * sj) ) : 0)
                                   + 0.5 * ((sj == 1 ? 0 : (si * si) / (sj * sj)) +(si == 1 ? 0 : (sj * sj) / (si * si)) - 2)));
                    */

                    case Mahalanobis:

                        return exp(-((mi-mj) * (2.0 / (si * si + sj * sj)) * (mi-mj)));


                   case Hellinger:

                        return exp(-sqrt(0.5 - sqrt(0.5 * (si * sj / (si * si + sj * sj)) * exp(-0.5 * (mi-mj) * (mi-mj) / (si * si + sj * sj)))));


                    default:

                        return 0;
                }

            default:

                return 0;
        }
}

/*!
    Computes steady state transition matrix (for StMerg).
    Inputs - (normalized) transition table
             log file if verbose.
    Returns - stationary distribution matrix.
*/

std::vector< std::vector<double> > Algo::ComputeSteadyStateTransition(const std::vector< std::vector<double> >& transition, int n, std::ofstream& outFile, bool verbose)
{
    std::vector< std::vector<double> > p = transition;
    std::vector<bool> deleted(n, false);
    double error;
    double err[MAX_STEPS];

    for (int i = 0; i < MAX_STEPS; ++i)
    {
        p = Multiply(p, transition, n);
        err[i] = error = Error(p, n);

        if (error < ERROR)
        {
            if (verbose)
            {
                outFile << "--------------------------------------------------------------------------------------------------------------------------" << std::endl;
                outFile << "Steady State Transition Table: Now Convergence after " << i << " iterations" << std::endl;
                outFile << "--------------------------------------------------------------------------------------------------------------------------" << std::endl;
                DumpTransitionTable(outFile, p, deleted);
                outFile << "--------------------------------------------------------------------------------------------------------------------------" << std::endl;
                outFile << "Error:" << error << std::endl;
                outFile << "--------------------------------------------------------------------------------------------------------------------------" << std::endl;
            }
            break;
        }
        else if (i == MAX_STEPS - 1)
        {
            std::vector<double> avg(n, 0);
            for (int j = 0; j < n; ++j)
            {
                for (int k = 0; k < n; ++k)
                {
                    avg[j] += p[k][j];
                }                
                p[0][j] = (avg[j] /= n);
            }
            if (verbose)
            {
                outFile << "--------------------------------------------------------------------------------------------------------------------------" << std::endl;
                outFile << "Steady State Transition Table: Convergence after " << i << " iterations" << std::endl;
                outFile << "--------------------------------------------------------------------------------------------------------------------------" << std::endl;
                DumpTransitionTable(outFile, p, deleted);
                outFile << "--------------------------------------------------------------------------------------------------------------------------" << std::endl;
                outFile << "Error:" << error << std::endl;
                outFile << "--------------------------------------------------------------------------------------------------------------------------" << std::endl;
            }
            break;
        }
    }
    outFile << "--------------------------------------------------------------------------------------------------------------------------" << std::endl;
    outFile << "Error:" << error << std::endl;
    for (int i = 0; i < MAX_STEPS; ++i)
    {
        outFile << err[i] << " ";
    }
    outFile << std::endl << "--------------------------------------------------------------------------------------------------------------------------" << std::endl;

    return p;
}

/*!
    Matrix multiplication
    Inputs - Two matrices
    Returns - Product of those two matrices.
*/

std::vector< std::vector<double> > Algo::Multiply(const std::vector< std::vector<double> >& a, const std::vector< std::vector<double> >& b, int n)
{
     std::vector< std::vector<double> > c(n, std::vector<double>(n));
     for (int i = 0; i < n; ++i)
     {
         for (int j = 0; j < n; ++j)
         {
             c[i][j] = 0;
             for (int k = 0; k < n; ++k)
             {
                c[i][j] += a[i][k] * b[k][j];
             }
         }
     }
     return c;
}

/*!
    Computes the convergence error (difference between any two rows) for stationary distribution
    Inputs - transition matrix
             row size (e.g., number of bins)
    Returns - Error to converge.
*/

double Algo::Error(const std::vector< std::vector<double> >& a, int n)
{
    double error = 0;
    for (int i = 0; i < n - 1; ++i)
    {
         for (int j = 0; j < n; ++j)
         {
              error +=  fabs(a[i][j] - a[i+1][j]);
         }
    }
    return error;
}

/*!
    Finds highest similar Markov states (equivalently bins) to merge (SMerg)
    Inputs - transition table
             number of bins
             threshold for merging
    Returns - true if any two states (bins) are mergable and their indices by reference
              false if no more mergable states (bins).
*/
bool Algo::FindStatesToMerge(std::vector< std::vector<double> >& transitionTable, int n, int& row, int& col, double threshold, std::ofstream& outFile, bool verbose)
{
    double max = -1;
    row = col = -1;

    for (int i = 0; i < n; ++i)
    {
           for (int j = i - 1; j <= i + 1; j += 2) // for (int j = i + 1; j < n; ++j)
           {
                if (j < 0 || j >= n)
                {
                    continue;
                }
                if ((transitionTable[i][j] + transitionTable[j][i]) / 2 > max)
                {
                    max = (transitionTable[i][j] + transitionTable[j][i]) / 2;
                    row = i;
                    col = j;
                }
           }
    }

    if (verbose)
    {
        outFile << std::endl << "--------------------------------------------------------------------------------------------------------------------------" << std::endl;
        outFile << "Found States (" << row << ", " << col << ") with maximum transition probability: " << max << std::endl;
    }

    if ((max > threshold)) // || (threshold - max < EPSILON))
    {
        if (verbose)
        {
            outFile << "Maximum transition probability: " << max /*within EPSILON from */ << " above threshold: " << threshold << ": Continue." << std::endl;
        }
    }
    else
    {
        if (verbose)
        {
            outFile << "Maximum transition probability: " << max /*within EPSILON from */ << " below threshold: " << threshold << ": Terminate." << std::endl;
        }
    }

    if (verbose)
    {
        outFile << "--------------------------------------------------------------------------------------------------------------------------" << std::endl;
    }

    return (max > threshold); // || (threshold - max < EPSILON);
}

/*!
    Finds highest similar Markov states (equivalently bins) to merge - for 2-D merging (image processing) - not in use
    Inputs - transition table
             number of bins
             threshold for merging
    Returns - true if any two states (bins) are mergable and their indices by reference
              false if no more mergable states (bins).
*/
bool Algo::FindStatesToMerge(std::vector< std::vector<double> >& transitionTable, int n, int& row, int& col, double threshold, std::ofstream& outFile, bool verbose, bool mergeImage, int w, int h)
{
    double max = -1;
    row = col = -1;

    std::vector<int> offset;
    if (mergeImage)
    {
        int a[] = {- w - 1, - w, - w + 1, - 1, + 1, + w - 1, + w,  + w + 1};
        std::copy (a, a + 8, std::back_inserter(offset));
    }
    else
    {
        int a[] = {- 1, + 1};
        std::copy (a, a + 2, std::back_inserter(offset));
    }

    for (int i = 0; i < n; ++i)
    {                   
           for (int k = 0; k < offset.size(); ++k)
           {
                int j = i + offset[k];
                if (j < 0 || j >= n)
                {
                    continue;
                }
                if ((transitionTable[i][j] + transitionTable[j][i]) / 2 > max)
                {
                    max = (transitionTable[i][j] + transitionTable[j][i]) / 2;
                    row = i;
                    col = j;
                }
           }
    }

    if (verbose)
    {
        outFile << std::endl << "--------------------------------------------------------------------------------------------------------------------------" << std::endl;
        outFile << "Found States (" << row << ", " << col << ") with maximum transition probability: " << max << std::endl;
    }

    if ((max > threshold)) // || (threshold - max < EPSILON))
    {
        if (verbose)
        {
            outFile << "Maximum transition probability: " << max /*within EPSILON from */ << " above threshold: " << threshold << ": Continue." << std::endl;
        }
    }
    else
    {
        if (verbose)
        {
            outFile << "Maximum transition probability: " << max /*within EPSILON from */ << " below threshold: " << threshold << ": Terminate." << std::endl;
        }
    }

    if (verbose)
    {
        outFile << "--------------------------------------------------------------------------------------------------------------------------" << std::endl;
    }

    return (max > threshold); // || (threshold - max < EPSILON);
}

/*!
    Merges two Markov states and corresponding rows / columns in the transition table
    Inputs - transition table
             indices of the states (rows / columns) to merge
             log file if verbose
*/

void Algo::MergeStates(std::vector< std::vector<double> >& transitionTable, std::vector<bool>& deleted, int i, int j, int n, std::ofstream& outFile, bool verbose)
{
        // Merge State i and State j to State min(i, j) and delete the State max(i, j)
        if (verbose)
        {
            outFile << std::endl << "--------------------------------------------------------------------------------------------------------------------------" << std::endl;
            outFile << "Merging States (" << i << ", " << j << ")" << std::endl;
            outFile << "Merged Final State: Min(" << i << ", " << j << ") = " << (i < j ? i : j) << std::endl;
            outFile << "Merging Markov transition probabilities" << std::endl;
        }

        // i, j will merge and become the same state
        transitionTable[i][j] = transitionTable[j][i] = 0;

        // Modify the In-Transitions to i, j
        // consider columns i, j
        for (int k = 0; k < n; ++k)
        {
            if (k == i || k == j)
            {
                continue;
            }
            transitionTable[k][i] = transitionTable[k][j] = (transitionTable[k][i] + transitionTable[k][j]) / 2;
            // transitionTable[k][i] = transitionTable[k][j] = (transitionTable[k][i] + transitionTable[k][j]);
            // transitionTable[k][i] = transitionTable[k][j] = (transitionTable[k][i] < transitionTable[k][j] ? transitionTable[k][i] : transitionTable[k][j]);
            // transitionTable[k][i] = transitionTable[k][j] = (transitionTable[k][i] > transitionTable[k][j] ? transitionTable[k][i] : transitionTable[k][j]);
        }

        // Modify the Out-Transitions from i, j
        // consider rows i, j
        for (int k = 0; k < n; ++k)
        {
            if (k == i || k == j)
            {
                continue;
            }
            transitionTable[i][k] = transitionTable[j][k] = (transitionTable[i][k] + transitionTable[j][k]) / 2;
            // transitionTable[i][k] = transitionTable[j][k] = (transitionTable[i][k] + transitionTable[j][k]);
            // transitionTable[i][k] = transitionTable[j][k] = (transitionTable[i][k] < transitionTable[j][k] ? transitionTable[i][k] : transitionTable[j][k]);
            // transitionTable[i][k] = transitionTable[j][k] = (transitionTable[i][k] > transitionTable[j][k] ? transitionTable[i][k] : transitionTable[j][k]);
        }

        // delete
        deleted[i > j ? i : j] = true;

        if (verbose)
        {
            outFile << "Deleting State : Max(" << i << ", " << j << ") = " << (i > j ? i : j) << " after merging" << std::endl;
            outFile << "Merging States (" << i << ", " << j << ") complete to obtain New Final State: " << (i < j ? i : j) << std::endl;
            outFile << "--------------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
        }
}

/*!
    Merges two bins
    Inputs - collection of bins
             indices of the bins to merge
             log file if verbose
    Returns - collection of bins after merging
*/
std::vector<TemporalData<double> >& Algo::MergeBins(std::vector<TemporalData<double> >& bins, int i, int j, std::ofstream& outFile, bool verbose)
{
    // Q_ASSERT(outFile != NULL);
    Q_ASSERT((i < (int) bins.size()) && (j < (int) bins.size()));
    Q_ASSERT(i != j);

    if (verbose)
    {
        outFile << std::endl << "--------------------------------------------------------------------------------------------------------------------------" << std::endl;
        outFile << "Merging corresponding Bins (" << i << ", " << j << ") with Ids (" << bins[i].GetIds()[0] << ", " << bins[j].GetIds()[0] << ")" << std::endl;
        outFile << "Size of bin before adding data: " << bins[i < j ? i : j].GetSize() << std::endl;
    }
    bins[i < j ? i : j].AddData(bins[i > j ? i : j]);
    std::vector<int> ids = bins[i > j ? i : j].GetIds();
    for (int k = 0; k < ids.size(); ++k)
    {
        bins[i < j ? i : j].AddId(ids[k]);
    }

    if (verbose)
    {
        outFile << "Size of bin after adding data: " << bins[i < j ? i : j].GetSize() << std::endl;
        outFile << "Number of Bins before deleting: " << bins.size() << std::endl;
        outFile << "Deleting Bin " << (i > j ? i : j) << std::endl;
    }

    bins.erase(bins.begin() + (i > j ? i : j));

    if (verbose)
    {
        outFile << "Number of Bins after deleting: " << bins.size() << std::endl;
        outFile << "--------------------------------------------------------------------------------------------------------------------------" << std::endl;
    }

    return bins;
}

/*!
    Computes maximum row sum
    Inputs - transition table
             row size
    Returns - maximum sum
*/
double Algo::MaxRowSum(const std::vector< std::vector<double> >& transitionTable, int n)
{
    double max = -1;
    for (int i = 0; i < n; ++i)
    {
        double rowSum = 0;
        for (int j = 0; j < n; ++j)
        {
            rowSum += transitionTable[i][j];
        }
        if (rowSum > max)
        {
            max = rowSum;
        }
    }
    return max;
}

/*!
    Finds the optimal partition recursively (DMerg)
    Generates the fully-paranthesized form of partition and returns as string
*/
QString OptimalPartition(std::vector< std::vector<int> >& s, int i, int j)
{
   if (j > i)
   {       
      return
                QString("(")
                .append(OptimalPartition(s, i, s[i][j]))
                .append(QString(","))
                .append(OptimalPartition(s, s[i][j] + 1, j))
                .append(QString(")"));        
   }
   else
   {
      return QString::number(i - 1);
   }
}

/*!
    Genrates the optimal partition (complete binary) tree recursively (DMerg - dynamic programming)
    Returns the tree
*/
node* CreateTree(std::vector< std::vector<double> >& m, std::vector< std::vector<int> >& s, int i, int j, node *n, double& avg)
{
   static int sumScore = 0, count = 0;
   static double avgScore = 0;
   if (!n)
   {
      n = new node;
      n->left = n->right = 0;
      n->start = n->end = -1;
      n->score = 0;
   }
   if (j > i)
   {
      n->start = i - 1;
      n->end = j - 1;
      n->score = m[s[i][j]][s[i][j] + 1];
      n->left = CreateTree(m, s, i, s[i][j], n->left, avg);
      n->right = CreateTree(m, s, s[i][j] + 1, j, n->right, avg);
   }
   else
   {
       n->start = n->end = i - 1;
       n->score = 1;
   }
   sumScore += n->score;
   ++count;
   avgScore = sumScore / (count * 1.0);
   avg = avgScore;
   return n;
}

/*!
    Pre-order traversal of the binary tree
    Input - root of the tree
*/
void PreOrder(std::ofstream& f, node *n)
{
    if (n)
    {
        f << "(" << n->start;
        if (n->end != n->start)
        {
            f << ", " << n->end;
        }
        f << ") " ;
        if (n->end != n->start)
        {
             f  << " " << n->score;
        }
        f << std::endl;
        PreOrder(f, n->left);
        PreOrder(f, n->right);
    }
}

/*!
    Breadth-first search of the binary tree
    Merge nodes if the similarity in between them is greater than threshold
    Input - root of the tree
            threshold
    Returns - splitpoints (by reference) after merging similar nodes
*/
void BFS(node *n, std::vector<int>& splitPoints, double threshold = 0.8)
{
    std::ofstream f("d:\\test10.txt");
    std::queue<node*> q;
    q.push(n);
    splitPoints.push_back(n->end + 1);
    while (!q.empty())
    {
        node *cn = q.front();
        if (cn->score > threshold)
        {
            f << "(" << cn->start;
            if (cn->end != cn->start)
            {
                f << ", " << cn->end;
            }
            f << ") ";
            splitPoints.push_back(cn->start);
        }
        else
        {
            if (cn->left)
            {
                q.push(cn->left);
            }
            if (cn->right)
            {
                q.push(cn->right);
            }
        }
        q.pop();
    }
    std::sort(splitPoints.begin(), splitPoints.end(), std::greater<int>());
    copy (splitPoints.begin(), splitPoints.end(), std::ostream_iterator<int>(f," "));
}

/*!
    In-order traversal of the binary tree
    Input - root of the tree
*/
void InOrder(std::ofstream& f, node *n)
{
    if (n)
    {
        InOrder(f, n->left);
        f << "(" << n->start;
        if (n->end != n->start)
        {
            f << ", " << n->end;
        }
        f << ") ";
        if (n->end != n->start)
        {
             f  << " " << n->score;
        }
        f << std::endl;
        InOrder(f, n->right);
    }
}

/*!
    Finds the optimal partition recursively, another variant (Not in use)
    Generates the fully-paranthesized form of partition and returns as string
*/
QString OptimalPartitionSim(std::vector< std::vector<double> >& m, std::vector< std::vector<int> >& s, int i, int j)
{
   if (j > i)
   {
      return
                QString("((")
                .append(OptimalPartitionSim(m, s, i, s[i][j]))
                .append(OptimalPartitionSim(m, s, s[i][j] + 1, j))
                .append(QString(")"))
                .append(QString::number(m[i][j] - 1.0 / (m[s[i][j]][s[i][j] + 1])))
                .append(QString(")"));
   }
   else
   {
      return "";
   }
}

/*!
    Runs different algorihtms for merging the bins to obtain unequal depth neighborhoods
    Inputs - collection of bins
             algorithm name
             threshold for merging
             distance metric
             window size
             log file if verbose
    Returns - unequal depth temporal neighborhood
*/
std::vector<TemporalData<double> >& Algo::RunMergeStates(std::vector<TemporalData<double> >& bins, Algorithm algoType, double thresholdLim, int minBinNum, int windowSize, Statistic statistic, DistanceMetric dMetric, NormalizationMetric nMetric, const char *logFile, bool verbose, bool mergeImage, int w, int h)
{
    Q_ASSERT(!verbose || (logFile != ""));
    std::ofstream outFile;

    if (verbose)
    {
        outFile.open(logFile, std::fstream::out);
    }

    int i, j, n = bins.size();
    Q_ASSERT(n > 1);

    std::vector<bool> deleted(n, false);

    if (algoType == Greedy)
    {
        int n = (int)bins.size();
        std::vector<double> means, sds;
        for(int i = 0; i < (int)bins.size(); ++i)
        {
            means.push_back(bins[i].GetStatistic(AVG));
            sds.push_back(bins[i].GetStatistic(SD));
        }
        std::ofstream f("d:\\test11.txt");
        std::vector<double> tr;
        //std::vector<int> pos;
        for (int i = 0; i < n - 1; ++i)
        {
            tr.push_back(ComputeMetric(i, i + 1, means, sds, dMetric, nMetric, windowSize));
            f << tr[i] << std::endl;
            //pos.push_back(i);
        }        
        if (minBinNum < 2)
        {
            minBinNum = 2;
        }
        double threshold = thresholdLim;
        std::vector<double>::iterator it = std::max_element(tr.begin(), tr.end());
        int i = it - tr.begin();
        while ((n > minBinNum) && (*it) > threshold)
        {
            MergeBins(bins, i, i + 1, outFile);
            tr.erase(it);
            --n;
            it = std::max_element(tr.begin(), tr.end());
            i = it - tr.begin();
        }
    }
    else if (algoType == DynamicProgramming)
    {
        std::vector<double> means, sds;

        for(int i = 0; i < (int)bins.size(); ++i)
        {
            means.push_back(bins[i].GetStatistic(AVG));
            sds.push_back(bins[i].GetStatistic(SD));
        }        

        std::vector< std::vector<double> > m(n + 1, std::vector<double>(n + 1));
        std::vector< std::vector<int> > s(n + 1, std::vector<int>(n + 1));

        for (int i = 1; i <= n; ++i)
        {
            for (int j = 1; j <= n; ++j)
            {
                if (i == j)
                {
                    m[i][j] = 0;
                }
                if (i - j == 1)
                {
                    m[i][j] = m[j][i] = ComputeMetric(i - 1, j - 1, means, sds, dMetric, nMetric, windowSize);
                    s[j][i] = j;
                }
            }
        }            

        std::ofstream f("d:\\test8.txt");

        for (int l = 2; l <= n; ++l) // l is chain length
        {
           for (int i = 1; i <= n - l + 1; ++i)
           {
                int j = i + l - 1;
                if (m[i][j] > 0)
                {
                    continue;
                }
                double d;
                d = m[i][j] = -9999;
                for (int k = i; k <= j - 1; ++k)
                {
                    double q = m[i][k] + m[k + 1][j];
                    // f << k - 1 << "," << k << " " << q  << std::endl;
                    double ds = - log(m[k][k + 1]);
                    // ds = log(ds);
                    if (/*q > m[i][j] && */ q + ds > d)
                    {
                        m[i][j] = q;
                        d = q + ds;
                        s[i][j] = k;
                    }
                }
                // f << i << "," << j << " " << s[i][j]  << std::endl;
            }
        }

        f << (OptimalPartition(s, 1, n)).toLatin1().data();
        // f << (OptimalPartitionSim(m, s, 1, n)).toLatin1().data();

        /*
        for (int i = 1; i <= n; ++i)
        {
            for (int j = i + 1; j <= n; ++j)
            {
                f << std::endl << "m[" << i - 1 << "," << j - 1 << "] = " << m[i][j];
            }
        }
        */

      node *t = new node;
      t->left = t->right = 0;
      t->start = 0;
      t->end = n - 1;
      double avg = 0;
      CreateTree(m, s, 1, n, t, avg);
      f << std::endl << "avg score: " << avg;
      f << std::endl; PreOrder(f, t);
      f << std::endl; InOrder(f, t);
      tr = t;
      std::vector<int> splitPoints;
      BFS(t, splitPoints, thresholdLim);
      copy (splitPoints.begin(), splitPoints.end(), std::ostream_iterator<int>(f," "));
      Q_ASSERT(splitPoints.size() >= 2);
      {
          int i = 0;
          std::vector<int>::iterator it = splitPoints.begin();
          int f = *it;
          int s = *(it + 1);
          while ((it + 1) != splitPoints.end())
          {
                for (int i = f - 1; i > s; --i)
                {
                    MergeBins(bins, i - 1, i, outFile);
                }
                ++it;
                f = *it;
                s = *(it + 1);
          }
      }
    }
    /*else if (algoType == SimilarityWithDFT)
    {
         steadyStateTransitionVector.clear();
         double sum = 0;
         for (int i = 0; i < n - 1; ++i)
         {
            double mi = bins[i].GetStatistic(AVG), mj = bins[i + 1].GetStatistic(AVG);
            double si = bins[i].GetStatistic(SD), sj = bins[i + 1].GetStatistic(SD);
            steadyStateTransitionVector.push_back(exp(-(0.5 * (mi - mj) * (mi - mj) / (2 * sj * sj)
                                   + (si ? log(sj / si) : 0) + 0.5 * ((si * si) / (sj * sj) - 1))));
            sum += steadyStateTransitionVector[i];
         }

         Q_ASSERT(sum > 0);

         for (i = 0 ; i < n; ++i)
         {
             steadyStateTransitionVector[i] /= sum;
         }

         std::ofstream f("d:\\test2.txt");

         DoFourierAndInverseFourierTransform(steadyStateTransitionVector, thresholdLim * 100, false);

         for (i = 0 ; i < n; ++i)
         {
            f << (steadyStateTransitionVector[i] >= 0 ? 1 : 0);
         }
         f << std::endl;

         i = j = 0;
         int k = 0;
         while (i < n - 1)
         {
            j = i;
            f << "B: " << k << std::endl;
            while ((j < n - 1) && ((steadyStateTransitionVector[k] * steadyStateTransitionVector[k + 1]) >= 0))
            {
                ++j;
                ++k;
            }
            f << "E: " << k << std::endl;
            while (i < j)
            {
                MergeBins(bins, i, i + 1, outFile);
                --j;
            }
            n = bins.size();
            ++i;
            ++k;
         }
         f.close();
    }*/
    else // if (algoType == SimilarityWithMarkovStationary || algoType == SimilarityWithTrMatrix)
    {
        std::vector<std::vector<double> > transitionTable = ComputeTransitionTable(bins, statistic, dMetric, nMetric, windowSize);

        if (verbose)
        {
            DumpTransitionTable(outFile, transitionTable, deleted);
        }

        if (algoType == SimilarityWithMarkovStationary)
        {
            /*if (thresholdLim > 1)
            {
                thresholdLim = EPSILON;
            }*/

            transitionTable = ComputeSteadyStateTransition(transitionTable, n, outFile);

            steadyStateTransitionVector.clear();
            for (i = 0; i < n; ++i)
            {
                steadyStateTransitionVector.push_back(transitionTable[0][i]);
            }

            if (!mergeImage)
            {
                std::ofstream f("d:\\test1.txt");

                DoFourierAndInverseFourierTransform(steadyStateTransitionVector, thresholdLim * 100, false);

                for (i = 0 ; i < n; ++i)
                {
                    f << steadyStateTransitionVector[i]; //(steadyStateTransitionVector[i] >= 0 ? 1 : 0);
                }
                f << std::endl;

                i = j = 0;
                int k = 0;
                while (i < n - 1)
                {
                    j = i;
                    f << "B: " << k << std::endl;
                    while ((j < n - 1) && ((steadyStateTransitionVector[k] * steadyStateTransitionVector[k + 1]) >= 0))
                    {
                        f << steadyStateTransitionVector[k] << "";
                        ++j;
                        ++k;
                    }
                    f << std::endl;
                    f << "E: " << k << std::endl;
                    while (i < j)
                    {
                        MergeBins(bins, i, i + 1, outFile);
                        --j;
                    }
                    n = bins.size();
                    ++i;
                    ++k;
                }
                f.close();
            }

            /*i = j = 0;
            while (i < n - 1)
            {
                //double md = 0, sdd = 0;
                while (fabs(transitionTable[0][i] - transitionTable[0][i + 1]) < thresholdLim)
                {
                    MergeBins(bins, j, j + 1, outFile);
                    ++i;
                    --n;
                    if (i >= n - 1)
                    {
                        break;
                    }
                }
                ++i;
                ++j;
            }*/
        }
        else // if (algoType != SimilarityWithMarkovStationary)
        {
            if (minBinNum < 2)
            {
                minBinNum = 2;
            }

            double threshold = THRESHOLD(thresholdLim);

            i = j = -1;

            // threshold = ComputeThreshold(transitionTable, n);

            while ((n > minBinNum) && (FindStatesToMerge(transitionTable, n, i, j, threshold, outFile, verbose, mergeImage, w, h)))
            {
                MergeBins(bins, i, j, outFile);
                transitionTable = ComputeTransitionTable(bins, statistic, dMetric, nMetric, windowSize);
                if (verbose)
                {
                    DumpTransitionTable(outFile, transitionTable, deleted);
                }
                n = bins.size();    // --n;

                Q_ASSERT(n > 1);
                threshold = THRESHOLD(thresholdLim);

                /* switch (dMetric)
                {
                    case Mean:
                    case Bhattacharya:
                    case KL:
                    case Mahalanobis:

                        break;

                    default:

                        break;
                } */
            }
        }
    }

    if (verbose)
    {
        std::ofstream outFile1("d:\\test3.txt");
        outFile1 << std::endl << "Merged Bins:" << std::endl;
        for (int i = 0; i < bins.size(); ++i)
        {
            outFile1 << "Bin " << i << ": ";
            std::vector<int> ids = bins[i].GetIds();
            for (int j = 0; j < ids.size(); ++j)
            {
                outFile1 << " " << ids[j];
            }
            outFile1 << std::endl;
        }
    }

    return bins;
}

/*!
    Compute Threshold (using mean +- 3. standard deviation)
    Inputs - transition table
             row size
             log file if verbose
    Returns - threshold
*/
double Algo::ComputeThreshold(const std::vector< std::vector<double> >& transitionTable, int n)
{
    double sum, sumsq, mean, sd;
    sum = sumsq = mean = sd = 0;
    for (int row = 0; row < n; ++row)
    {
        for (int col = 0; col < n; ++col)
        {
            sum += transitionTable[row][col];
            sumsq += transitionTable[row][col] * transitionTable[row][col];
        }
    }
    mean = sum / (n * n);
    sd = sqrt((sumsq / (n * n)) - (mean * mean));
    return mean + sd;
}

/*!
    Does (Discrete) Fourier and Inverse Fourier transform on the temporal data after doing low/hi pass filter
    Used in StMerg
    Inputs - temporal data
             number of Fourier coefficient to use for IDFT
             low / hi pass filter
    Returns - temporal data after low / hi pass filter
*/
TemporalData<double>& Algo::DoFourierAndInverseFourierTransform(TemporalData<double>& temporalData, int k, bool lowPass)
{
    std::vector<double> data = temporalData.GetData();

    DoFourierAndInverseFourierTransform(data, k, lowPass);

    temporalData.Clear();
    temporalData.SetData(temporalData.GetIds()[0], data);

    return temporalData;
}

/*!
    Does (Discrete) Fourier and Inverse Fourier transform on the temporal data after doing low/hi pass filter
    Used in StMerg
    Inputs - temporal data
             number of Fourier coefficients to use for IDFT
             low / hi pass filter
    Returns - temporal data after low / hi pass filter
*/
std::vector<int>& Algo::DoFourierAndInverseFourierTransform(std::vector<int>& data, int k, bool lowPass)
{
    int n = data.size();
    std::vector<int> fReal, fImg;
    double nFuRe, nFuImg;

    for (int u = 0; u < n; ++u)
    {
        nFuRe = nFuImg = 0;
        for (int x = 0; x < n; ++x)
        {
            nFuRe += data[x] * cos(2 * M_PI * u * x / n);
            nFuImg -= data[x] * sin(2 * M_PI * u * x / n);
        }
        fReal.push_back(nFuRe);
        fImg.push_back(nFuImg);
    }

    if (k <= 0)
    {
        k = n;
    }

    data.clear();

    double nIRe, nIIm;
    for (int x = 0; x < n; ++x)
    {
        nIRe = nIIm = 0;
        int l = lowPass ? k : n;
        for (int u = lowPass ? 0 : n - k; u < l; ++u)
        {
            nIRe += cos(2 * M_PI * u * x / n) * fReal[u] - sin(2 * M_PI * u * x / n) * fImg[u];
            nIIm += sin(2 * M_PI * u * x / n) * fReal[u] - cos(2 * M_PI * u * x / n) * fImg[u];
        }
        data.push_back(nIRe / k); //(sqrt(nIRe * nIRe + nIIm * nIIm) / k);
    }

    return data;
}

/*!
    Does (Discrete) Fourier and Inverse Fourier transform on the temporal data after doing low/hi pass filter
    Used in StMerg
    Inputs - temporal data
             number of Fourier coefficients to use for IDFT
             low / hi pass filter
    Returns - temporal data after low / hi pass filter
*/
std::vector<double>& Algo::DoFourierAndInverseFourierTransform(std::vector<double>& data, int k, bool lowPass)
{
    int n = data.size();
    std::vector<double> fReal, fImg;
    double nFuRe, nFuImg;

    for (int u = 0; u < n; ++u)
    {
        nFuRe = nFuImg = 0;
        for (int x = 0; x < n; ++x)
        {
            nFuRe += data[x] * cos((2.0 * M_PI * u * x) / n);
            nFuImg -= data[x] * sin((2.0 * M_PI * u * x) / n);
        }
        fReal.push_back(nFuRe);
        fImg.push_back(nFuImg);
    }

    if (k <= 0)
    {
        k = n;
    }

    data.clear();

    double nIRe, nIIm;
    for (int x = 0; x < n; ++x)
    {
        nIRe = nIIm = 0;
        int l = lowPass ? k : n;
        for (int u = lowPass ? 0 : n - k; u < l; ++u)
        {
            nIRe += cos(2 * M_PI * u * x / n) * fReal[u] - sin(2 * M_PI * u * x / n) * fImg[u];
            nIIm += sin(2 * M_PI * u * x / n) * fReal[u] - cos(2 * M_PI * u * x / n) * fImg[u];
        }
        data.push_back(nIRe / k); // sqrt(nIRe * nIRe + nIIm * nIIm) / k;
    }

    return data;
}

/*!
    Does (Discrete) Cosine and Inverse Cosine transform on the temporal data after doing low/hi pass filter
    Inputs - temporal data
             number of Cosine coefficients to use for IDCT
             low / hi pass filter
    Returns - temporal data after low / hi pass filter
*/
TemporalData<double>& Algo::DoCosineAndInverseCosineTransform(TemporalData<double>& temporalData, int k, bool lowPass)
{
    std::vector<double> data = temporalData.GetData();

    DoCosineAndInverseCosineTransform(data, k, lowPass);

    temporalData.Clear();
    temporalData.SetData(temporalData.GetIds()[0], data);

    return temporalData;
}

/*!
    Does (Discrete) Cosine and Inverse Cosine transform on the temporal data after doing low/hi pass filter
    Inputs - temporal data
             number of Cosine coefficients to use for IDCT
             low / hi pass filter
    Returns - temporal data after low / hi pass filter
*/
std::vector<int>& Algo::DoCosineAndInverseCosineTransform(std::vector<int>& data, int k, bool lowPass)
{
    int n = data.size();
    std::vector<int> vCos;
    double nCos;

    for (int u = 0; u < n; ++u)
    {
        nCos = 0;
        for (int x = 0; x < n; ++x)
        {
            nCos += data[x] * cos(M_PI * (2 * x + 1) * u / (2.0 * n));
        }
        vCos.push_back(nCos * (u ? 2.0 / sqrt(n) : 1.0 / sqrt(n)));
    }

    if (k <= 0)
    {
        k = n;
    }

    data.clear();

    double nICos;
    for (int x = 0; x < n; ++x)
    {
        nICos = 0;
        int l = lowPass ? k : n;
        for (int u = lowPass ? 0 : n - k; u < l; ++u)
        {
            nICos += (u ? 2.0 / sqrt(n) : 1.0 / sqrt(n)) * cos((M_PI * (2 * x + 1) * u) / (2.0 * n)) * vCos[u];
        }
        data.push_back(nICos);
    }

    return data;
}

/*!
    Does (Discrete) Cosine and Inverse Cosine transform on the temporal data after doing low/hi pass filter
    Inputs - temporal data
             number of Cosine coefficients to use for IDCT
             low / hi pass filter
    Returns - temporal data after low / hi pass filter
*/
std::vector<double>& Algo::DoCosineAndInverseCosineTransform(std::vector<double>& data, int k, bool lowPass)
{
    int n = data.size();
    std::vector<int> vCos;
    double nCos;

    for (int u = 0; u < n; ++u)
    {
        nCos = 0;
        for (int x = 0; x < n; ++x)
        {
            nCos += data[x] * cos(M_PI * (2 * x + 1) * u / (2.0 * n));
        }
        vCos.push_back(nCos * (u ? 2.0 / sqrt(n) : 1.0 / sqrt(n)));
    }

    if (k <= 0)
    {
        k = n;
    }

    data.clear();

    double nICos;
    for (int x = 0; x < n; ++x)
    {
        nICos = 0;
        int l = lowPass ? k : n;
        for (int u = lowPass ? 0 : n - k; u < l; ++u)
        {
            nICos += (u ? 2.0 / sqrt(n) : 1.0 / sqrt(n)) * cos((M_PI * (2 * x + 1) * u) / (2.0 * n)) * vCos[u];
        }
        data.push_back(nICos);
    }

    return data;
}

/*!
    Experiments
    Set of experiments are there - uncomment only one of them at a time to do the experiment
*/
void Algo::DoExperiment()
{
    /* std::ofstream outFile("d:\\results.txt");

    for (int n = 2; n <= 100; ++ n)
    {
        outFile << "n = " << n;
        int count = 0;
        for (int x = 1; x < 2 * n; ++x)
        {
            for (int y = 2 * n + 1; y < 100000; ++y)
            {
                if (n * (x + y) == x * y)
                {
                    outFile << " (" << x << ", " << y << ")";
                    ++count;
                }
            }
        }
        outFile << " count = " << count << std::endl << std::endl;
    } */

    /*std::vector<int> primes;
    std::ofstream outFile("d:\\results.txt");

    for (int i = 2; i < 1000; ++i)
    {
        bool prime = true;
        for (int j = 2; j < sqrt(i); ++j)
        {
            if (!(i % j))
            {
                prime = false;
            }
        }
        if (prime)
        {
            primes.push_back(i);
        }
    }

    for (int n1 = 1; n1 < 100; ++n1)
    {
    for (int n2 = n1; n2 < 100; ++n2)
    {
    for (int n3 = n2; n3 < 100; ++n3)
    {
    for (int n4 = n3; n4 < 100; ++n4)
    {
        int n = n1 + n2 + n3 + n4;
        std::vector<int>::iterator it = std::find(primes.begin(), primes.end(), n);
        if (it != primes.end())
        {
            outFile << n << " " << n1 << " " << n2 << " " << n3 << " " << n4 << std::endl;
        }
    }
    }
    }
    } */

    /**/
    std::ofstream outFile("d:\\results.csv");

    TemporalData<double> temporalData(0);
    std::vector<TemporalData<double> > bins;
    /**/

    /*
    outFile << "              , Bhattacharyya,        ,       , KL         ,          ,       , Mahalanobis,          ,       , Hellinger  ,          ,       " << std::endl;
    outFile << "Iniitial Bin # (n)";
    // outFile << ", Final Bin #, Precision, Recall, Final Bin #, Precision, Recall, Final Bin #, Precision, Recall, Final Bin #, Precision, Recall" << std::endl;
    for (double threshold = 0.55; threshold <= 0.95; threshold += 0.05)
    {
        outFile << "," << threshold;
    }
    outFile << std::endl;
    /*SetSeed(time(NULL)); //(i << 24 + i << 16 + i << 8 + i);

    std::ifstream file("d:\\catt-lab-count-4_12-4_17.txt");
    std::vector<int> timeSeriesData;
    char buf[256];
    while (file)
    {
        file.getline(buf, 256);
        char *pch = strtok (buf," ,-");
        while (pch != NULL)
        {
            timeSeriesData.push_back(atof(pch));
            pch = strtok (NULL, " ,-");
        }
    }
    temporalData.SetData(timeSeriesData);    

    int n = timeSeriesData.size();
    for (thresholdLim = 0.1; thresholdLim <= 2.0; thresholdLim += 0.1)
    {
        outFile << THRESHOLD(thresholdLim) << ",";
        for (int d = KL; d != Chernoff; ++d)
        {
             bins = PartitionInBins(temporalData, n, EqualFrequency);
             bins = RunMergeStates(bins, false, thresholdLim, 2, 5, AVG, (DistanceMetric)d, SSD, "", false);
             outFile << bins.size() << ",";
             //temporalData.Clear();
             //bins.clear();
        }
        outFile << std::endl;
    }
    */
    /*
    SetSeed(time(NULL)); //(i << 24 + i << 16 + i << 8 + i);
    dataSet = 0;

    std::vector<int> N;

    // N.push_back(100);
    // N.push_back(200);
    N.push_back(500);
    N.push_back(1000);
    N.push_back(2000);
    N.push_back(5000);
    N.push_back(7500);
    N.push_back(10000);
    N.push_back(20000);
    N.push_back(50000);
    N.push_back(75000);
    N.push_back(100000);
    N.push_back(200000);
    N.push_back(500000);
    N.push_back(1000000);
    N.push_back(2000000);
    N.push_back(5000000);
    N.push_back(7500000);
    N.push_back(10000000);
    N.push_back(20000000);

    std::ofstream outFile1("d:\\test4.txt");

    int n = 100;   
    Algorithm algorithm = DynamicProgramming;  // SimilarityWithTrMatrix; //  SimilarityWithMarkovStationary; //  // DynamicProgramming;
    double threshold = 0.9; // 1.0; // 1.0; // 0.1; // 0.3; // 0.95; // 0.5;
    int w = 99; // 5; // n;
    outFile << "Data, Final Bin" << std::endl;
    for (int i = 0; i < N.size(); ++i)
    {
        outFile << N[i] << ",";
        for (int d = Bhattacharya; d <= Hellinger; ++d)
        {
            temporalData = GenerateRandomData(N[i], None);
            bins = PartitionInBins(temporalData, n);
            QTime t;
            t.start();
            bins = RunMergeStates(bins, algorithm, threshold, 2, w, AVG, (DistanceMetric)d, SSD, "", false);
            outFile << t.elapsed() << ",";
            /*
            int merged = n - bins.size();
            int correct = 0, wrong = 0;
            int expected = n - 5;
            int j = 0;

            outFile1 << "Bin size: " << bins.size() << std::endl;

            int left[] = {0, 10, 30, 50, 70};
            int right[] = {9, 29, 49, 69, 99};

            for (int i = 0; i < bins.size(); ++i)
            {
                int binSize = bins[i].GetIds().size();
                std::vector<int> ids = bins[i].GetIds();
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
                        if (ids[0] > left[j])
                        {
                            ++j;
                        }
                        if (ids[0] < left[j])
                        {
                            wrong += left[j] - ids[0];
                        }
                        if (ids[binSize - 1] > right[j])
                        {
                            wrong += ids[binSize - 1] - right[j];
                        }
                    }
                }
                outFile1 << "Size of bin " << i << ": " << binSize << std::endl;
                //wrong +=
                
                //outFile1 << "correct: " << correct << "current: " << current << std::endl;
            }
            correct = n - bins.size() - wrong;

            outFile << bins.size() << "," << (correct / (merged * 1.0)) * 100 << "," << (correct / (expected * 1.0)) * 100 << ",";
            */
        /*
            temporalData.Clear();
            bins.clear();
        }
        outFile << std::endl;
    }
    */

    /*
    std::ofstream outFile1("d:\\test4.txt");

    std::vector<int> n;

    // N.push_back(100);
    // N.push_back(200);
    n.push_back(10);
    n.push_back(20);
    n.push_back(30);
    n.push_back(40);
    n.push_back(50);
    n.push_back(60);
    n.push_back(70);
    n.push_back(80);
    n.push_back(90);
    n.push_back(100);
    n.push_back(200);
    n.push_back(120);
    n.push_back(150);
    n.push_back(180);
    n.push_back(200);
    //n.push_back(500);

    int N = 1000;
    Algorithm algorithm = DynamicProgramming;  // SimilarityWithTrMatrix; //  SimilarityWithMarkovStationary; // ; // DynamicProgramming
    double threshold =  0.7; // 0.1; // 1.0; // 0.3; // 0.5; // 0.95;

    //for (int k = 0; k < 2; ++k)
    //{
        SetSeed(time(NULL)); //(i << 24 + i << 16 + i << 8 + i);
        dataSet = 0;
        int i = 0;
        for (int i = 0; i < n.size(); ++i)
        {
            outFile << n[i] << ",";
            //for (int d = Bhattacharya; d <= Hellinger; ++d)
            int d = KL;
            for (threshold = 0.55; threshold <= 0.95; threshold += 0.05)
            //for (int w = 5; w <= n[i]; w += 2)
            {
                //outFile << w << ",";
                //for (int d = Bhattacharya; d <= Hellinger; ++d)
                //{
                temporalData = GenerateRandomData(N, None); // None
                bins = PartitionInBins(temporalData, n[i]);
                bins = RunMergeStates(bins, algorithm, threshold, 2, n[i], AVG, (DistanceMetric)d, SSD, "", false);

                int merged = n[i] - bins.size();
                int correct = 0, wrong = 0;
                int expected = n[i] - 5;
                int j = 0;

                //outFile1 << "Bin size: " << bins.size() << std::endl;

                int left[] = {0, 10, 30, 50, 70};
                int right[] = {9, 29, 49, 69, 99};

                left[0] = 0; right[0] = n[i] / 10 - 1;
                left[1] = n[i] / 10; right[1] = 3 * n[i] / 10 - 1;
                left[2] = 3 * n[i] / 10; right[2] = 5 * n[i] / 10 - 1;
                left[3] = 5 * n[i] / 10; right[3] = 7 * n[i] / 10 - 1;
                left[4] = 7 * n[i] / 10; right[4] = n[i] - 1;

                for (int i = 0; i < bins.size(); ++i)
                {
                    int binSize = bins[i].GetIds().size();
                    std::vector<int> ids = bins[i].GetIds();
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
                            if (ids[0] > left[j])
                            {
                                ++j;
                            }
                            if (ids[0] < left[j])
                            {
                                wrong += left[j] - ids[0];
                            }
                            if (ids[binSize - 1] > right[j])
                            {
                                wrong += ids[binSize - 1] - right[j];
                            }
                        }
                    }
                    outFile1 << "Size of bin " << i << ": " << binSize << std::endl;
                }
                correct = n[i] - bins.size() - wrong;

                //outFile << bins.size() << ",";
                //outFile << (correct / (merged * 1.0)) * 100 << ",";
                outFile << (correct / (expected * 1.0)) * 100 << ",";

                temporalData.Clear();
                bins.clear();
            }
            outFile << std::endl;
        }
        outFile << std::endl << std::endl;
    //}    
    */

    /*
    std::ofstream outFile1("d:\\test4.txt");

    int N = 1000, n = 100;
    Algorithm algorithm = Greedy;  // SimilarityWithTrMatrix; //  SimilarityWithMarkovStationary; // Greedy; // DynamicProgramming;

    for (double threshold = 0.3; threshold <= 1.0; threshold += 0.025)
    {        
        for (int d = Bhattacharya; d <= Hellinger; ++d)
        {
            temporalData = GenerateRandomData(N, None); // None Gaussian
            bins = PartitionInBins(temporalData, n);
            int k = n - threshold * 40;
            if (!(k % 2)) --k;
            bins = RunMergeStates(bins, algorithm, threshold, 2,  k, AVG, (DistanceMetric)d, SSD, "", false);
            outFile << bins.size() << ",";

            int merged = n - bins.size();
            int correct = 0, wrong = 0;
            int expected = n - 5;
            int j = 0;

            //outFile1 << "Bin size: " << bins.size() << std::endl;

            int left[] = {0, 10, 30, 50, 70};
            int right[] = {9, 29, 49, 69, 99};

            for (int i = 0; i < bins.size(); ++i)
            {
                int binSize = bins[i].GetIds().size();
                std::vector<int> ids = bins[i].GetIds();
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
                        if (ids[0] > left[j])
                        {
                            ++j;
                        }
                        if (ids[0] < left[j])
                        {
                            wrong += left[j] - ids[0];
                        }
                        if (ids[binSize - 1] > right[j])
                        {
                            wrong += ids[binSize - 1] - right[j];
                        }
                    }
                }
                //outFile1 << "Size of bin " << i << ": " << binSize << std::endl;
            }

            correct = merged - wrong;
            outFile << threshold << "," << bins.size() << "," << (correct / (merged * 1.0)) * 100 << "," << (correct / (expected * 1.0)) * 100 << ",";

            temporalData.Clear();
            bins.clear();
        }
        outFile << std::endl;
    }
    */

    //std::ofstream outFile1("d:\\test4.txt");


    int N = 1000, n = 100;
    Algorithm algorithm = DynamicProgramming;  // SimilarityWithTrMatrix; //  SimilarityWithMarkovStationary; // SimilarityWithDFT;
    double threshold =  1.0; // 0.1; // 1.0; // 0.3;

    for (int n = 10; n <= 200; n += 10)
    {
        for (double threshold = 0.5; threshold <= 0.9; threshold += 0.05)
        {
            for (int d = Bhattacharya; d <= Bhattacharya/*Hellinger*/; ++d)
            {
                temporalData = GenerateRandomData(N, None); // None Gaussian
                bins = PartitionInBins(temporalData, n);
                bins = RunMergeStates(bins, algorithm, threshold, 2,  9, AVG, (DistanceMetric)d, SSD, "", false);
                //outFile << bins.size() << ",";

                int merged = n - bins.size();
                int correct = 0, wrong = 0;
                int expected = n - 5;
                int j = 0;

                //outFile << "Bin size: " << bins.size() << std::endl;

                int left[] = {0, 10, 30, 50, 70};
                int right[] = {9, 29, 49, 69, 99};

                for (int i = 0; i < bins.size(); ++i)
                {
                    int binSize = bins[i].GetIds().size();
                    std::vector<int> ids = bins[i].GetIds();
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
                            if (ids[0] > left[j])
                            {
                                ++j;
                            }
                            if (ids[0] < left[j])
                            {
                                wrong += left[j] - ids[0];
                            }
                            if (ids[binSize - 1] > right[j])
                            {
                                wrong += ids[binSize - 1] - right[j];
                            }
                        }
                    }
                    //outFile << "Size of bin " << i << ": " << binSize << std::endl;
                }

                correct = merged - wrong;
                outFile << bins.size()  << "," << threshold << "," << (correct / (merged * 1.0)) * 100 << "," << (correct / (expected * 1.0)) * 100 << ",";

                temporalData.Clear();
                bins.clear();
            }
            outFile << std::endl;
        }
    }
    
    /*outFile << "Data, Initial, , Final BinSize, Final BinSize, Final BinSize" << std::endl;
    outFile << "Size, BinSize, Bhattacharya, KL, Mahalanobis, Mean, Bhattacharya, KL, Mahalanobis, Mean, Bhattacharya, KL, Mahalanobis, Mean" << std::endl;
    //for (int N = 1000; N <= 1000; N += 1000) // (int N = 500; N <= 1000; N += 100)
    //{
        for (int n = 10; n <= N / 5; n += 20) // (int n = 10; n <= N / 5; n += 10)
        {
            outFile << N << "," << n << ",";
            for (int d = KL; d != Chernoff; ++d)
            {
                //for (int i = 0; i < 5; ++i)
                //{
                     temporalData = GenerateRandomData(N, Gaussian);
                     bins = PartitionInBins(temporalData, n, EqualFrequency);
                     bins = RunMergeStates(bins, false, 1.0, 2, 5, AVG, (DistanceMetric)d, SSD, "", false);
                     outFile << bins.size() << ",";
                     temporalData.Clear();
                     bins.clear();
                //}
            }
            outFile << std::endl;
        }
    }*/

    /*
     std::ifstream inFile1("d:\\cover.html");

     QString str = "";
     char c;
     int map[512] = {0};
     while (inFile1)
     {
        c = inFile1.get();
        map[c]++;
        str += c;
     }

     char msg[32];
     strcpy(msg, "Copyright @ Sandipan Dey");
     QBitArray ba(200);

     for (int i = 0, j = 0; i < strlen(msg); ++i)
     {
         int c = msg[i];
         for (int k = 0; k < 8; ++k)
         {
            ba[j++] = (c >> (7 - k)) & 0x01;
         }
     }

     inFile1.close();;

     std::ofstream outFile2("d:\\stego.html");

     QRegExp rx("<.+>");
     int pos = 0;    // where we are in the string
     int count = 0;
     int j = 0;

     while (pos >= 0) {

         int pos1 = pos;
         pos = rx.indexIn(str, pos);
         outFile2 << str.mid(pos1, pos - pos1).toLatin1().data();

         if (pos >= 0) {

             ++count;    // count
             while ((c = str.at(pos).toAscii()) != '>')
             {
               if (c == '"')
               {
                   outFile2 << c;
                   do
                   {
                       c = str.at(++pos).toAscii();
                       outFile2 << c;
                   }
                   while (c != '"');
                   ++pos;
               }
               else
               {
                   if (isalpha(c) && (j < 200))
                   {
                        c = (ba[j++] == 0) ? tolower(c) : toupper(c);
                   }
                   outFile2 << c;
                   ++pos;
               }
             }
         }
     }
     //outFile1 << count;

     outFile2.close();

     std::ifstream inFile2("d:\\stego.html");

     int tmap[512] = {0};
     while (inFile2)
     {
        c = inFile2.get();
        tmap[c]++;
        str += c;
     }

     inFile2.close();

     std::ofstream outFile1("d:\\test5.txt");
     for (int i = 0; i < 512; ++i)
     {
         outFile1 << map[i] << "    " << tmap[i] << std::endl;
     }
     for (int i = 0; i < 200; ++i)
     {
         outFile1 << ba[i];
     }

     outFile1.close();
     */
}

/*!
    Dumps the transition table to a file / console
    Inputs - transition table
             log file if verbose
*/
void Algo::DumpTransitionTable(std::ofstream& outFile, const std::vector<std::vector<double> >& table, std::vector<bool>& deleted) const
{
    int row = 0, col = 0;

    outFile << std::setprecision(PROB_PRECS);
    outFile << std::endl << "--------------------------------------------------------------------------------------------------------------------------" << std::endl;
    outFile << "Markov Probability Transition Table (pij)" << std::endl;
    outFile << "--------------------------------------------------------------------------------------------------------------------------" << std::endl;

    std::vector< std::vector<double> >::const_iterator row_it = table.begin();
    std::vector< std::vector<double> >::const_iterator row_end = table.end();

    for ( ; row_it != row_end; ++row_it, ++row)
    {
        if (deleted[row])
        {
            continue;
        }

        col = 0;
        std::vector<double>::const_iterator col_it = row_it->begin();
        std::vector<double>::const_iterator col_end = row_it->end();

        if (row_it == table.begin())
        {
            outFile << " ";
            outFile << std::setw (TABLE_SPACE);
            for ( ; col_it != col_end; ++col_it, ++col)
            {
                if (!deleted[col])
                {
                    outFile << std::setw (TABLE_SPACE) << col;
                }
            }
            outFile << std::endl;
            col_it = row_it->begin();
        }
        // outFile << "--------------------------------------------------------------------------------------------------------------------------" << std::endl;

        col = 0;
        // outFile << row << std::setw (TABLE_SPACE);
        for ( ; col_it != col_end; ++col_it, ++col)
        {
            if (deleted[col])
            {
                continue;
            }
            outFile << std::setw (TABLE_SPACE) << *col_it;
        }

        outFile << std::endl;        
    }
    outFile << "--------------------------------------------------------------------------------------------------------------------------" << std::endl;
}

/*!
    Sets the seed for (pseudo) random generator
*/
void Algo::SetSeed(int seed)
{
    this->seed = seed;
    dataSet = (dataSet + 1) % 6;
}
