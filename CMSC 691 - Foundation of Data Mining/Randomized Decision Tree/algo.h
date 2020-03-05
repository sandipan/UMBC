/* // ----------------------------------------------------------------
   //                   Algortihm Class
   // ----------------------------------------------------------------
   //                   Contains the algorithms for RDT
   // ----------------------------------------------------------------
   //                   Sandipan Dey
   //                   Graduate Student
   //                   UMBC CSEE
   // ----------------------------------------------------------------*/

#ifndef ALGO_H
#define ALGO_H

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <QPainter>
#include <QString>

/* Tree node */
struct node
{
    std::string feature;
    std::vector<node*> next;
    std::map<std::string, int> classdist;
};

/*!
    Algo class
    Implements all the algorithms
*/
class Algo
{
    public:

    std::string dataFile;
    std::vector<std::string> classes, features;
    std::map<std::string, std::vector<std::string> > featurevalues;
    std::vector<double> max, min;
    int NT;
    std::vector<node*> RDT;
    std::vector<double> w;
    std::vector<double> acc;
    double accavg;
    QString status;

    Algo();
    std::vector<std::string> GenAllTuples();
    void PreProcess(const std::string&);
    void PreProcessEARDMS();
    void PreProcessBinary(const std::string&);
    double LoadDataAndTrainTest(const std::string&, int, int, bool = true, double = 1.0);
    double LoadDataAndTrainWeightedTest(const std::string&, int, int, int, bool = true, double = 1.0);
    void Initialize(const std::string&);
    void Cleanup();
    void ReadData(bool, int, double);
    void ReadNames();
    void RandomSample(const std::vector<std::string>&, int, double, bool = true);
    void TrainRDTs(bool);
    double TestRDTs();
    void Train(std::ifstream&, node*, bool = true);
    double Test(std::ifstream&, std::ofstream&);
    void BuildRDTs(int, int);
    node* BuildTreeStructure(node*, int, int, const std::vector<std::string>&);
    void DeleteTreeStructure(node*);
    void UpdateStatistics(node*, std::map<std::string, std::string>&, std::string&);
    std::map<std::string, int>& Classify(node*, std::map<std::string, std::string>&);
    std::map<std::string, int>& Match(node*, std::map<std::string, std::string>&);
    void BuildFourierCoefficientVector();
    void DrawTree(QPainter&, node*, int, int, int);
    void PreOrder(node*, std::ofstream&, std::string);
    void RunTests(const std::string&);
};

#endif // ALGO_H
