/* // ------------------------------------------------------------
   //                   TemporalData Class
   // ------------------------------------------------------------
   // Stores the temporal data objects containing time series data
   // ------------------------------------------------------------
   //                   Sandipan Dey
   //                   Graduate Student
   //                   UMBC
   // ------------------------------------------------------------ */

#ifndef DATA_H
#define DATA_H

#include <vector>
#include <string>
#include "types.h"

/*!
    TemporalData class
    Impelements Data Storage
*/
template <class T>
class TemporalData
{
    // Time
    std::vector<std::string> time;
    // Time Series Data
    std::vector<T> data;
    // Aggregate Data / Statistics
    std::vector<double> statistics;  // std::vector<T> statistics;

    std::vector<int> ids;

public:

    // Constructors
    TemporalData(int);
    TemporalData(int, typename std::vector<T>::const_iterator, typename std::vector<T>::const_iterator);
    TemporalData(int, typename std::vector<T>::const_iterator, typename std::vector<T>::const_iterator, std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    TemporalData(const TemporalData<T>&);
    TemporalData& operator=(const TemporalData<T>&);

    // Initialize Data & Statistics
    void Init(int);

    // Get Methods
    std::vector<std::string> GetTime() const;
    std::vector<T> GetData() const;
    std::vector<int> GetIds() const;
    int GetSize() const;

    double /*T*/ GetStatistic(Statistic) const;
    std::vector<double /*T*/ > GetStatistics() const;


    // Set Methods
    void SetData(int id, const std::vector<T>&);
    void SetTime(const std::vector<std::string>&);
    void SetStatistics(const std::vector<double /*T*/ >&);
    void SetStatistic(Statistic, double /*T*/);
    void AddData(const TemporalData<T>&);
    void AddId(int);

    // Computation of statistics
    void ComputeStatistics();

    // Serialize/deserialize
    void Serialize(const char*, FileFormat);
    void Deserialize(const char*, FileFormat);

    // Clear
    void Clear();
};

#include "data.cpp" // hack for template instantiation linking problem

#endif // DATA_H
