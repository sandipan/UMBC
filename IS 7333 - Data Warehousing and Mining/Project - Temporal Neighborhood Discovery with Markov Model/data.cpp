/*! // ------------------------------------------------------------
   //                   TemporalData Class
   // ------------------------------------------------------------
   // Stores the temporal data objects containing time series data
   // ------------------------------------------------------------
   //                   Sandipan Dey
   //                   Graduate Student
   //                   UMBC
   // ------------------------------------------------------------ */

#include <QMessageBox>
#include <algorithm>
#include <fstream>
#include <math.h>
#include "data.h"

#define MAX_BUF 256
#define MAX_INIT -9999
#define MIN_INIT (1 << 30)
#define SUM_INIT 0
#define AVG_INIT 0
#define SD_INIT 0

template <class T>
TemporalData<T>::TemporalData(int id)
{
    Init(id);
}

template <class T>
TemporalData<T>::TemporalData(int id, typename std::vector<T>::const_iterator it_begin, typename std::vector<T>::const_iterator it_end)
{
    Init(id);
    data.assign(it_begin, it_end);
    ComputeStatistics();
}


template <class T>
TemporalData<T>::TemporalData(int id, typename std::vector<T>::const_iterator it_begin, typename std::vector<T>::const_iterator it_end, std::vector<std::string>::const_iterator itt_beg, std::vector<std::string>::const_iterator itt_end)
{
    Init(id);
    data.assign(it_begin, it_end);
    ComputeStatistics();
    time.assign(itt_beg, itt_end);
}

template <class T>
TemporalData<T>::TemporalData(const TemporalData<T>& d)
{
    std::vector<int> ids = d.GetIds();
    SetData(ids[0], d.GetData());
    for (int i = 1; i < ids.size(); ++i)
    {
        AddId(ids[i]);
    }
    SetTime(d.GetTime());
}

template <class T>
TemporalData<T>& TemporalData<T>::operator=(const TemporalData<T>& d)
{
    std::vector<int> ids = d.GetIds();
    SetData(ids[0], d.GetData());
    for (int i = 1; i < ids.size(); ++i)
    {
        AddId(ids[i]);
    }
    SetTime(d.GetTime());
    return *this;
}

template <class T>
void TemporalData<T>::Init(int id)
{
    data.clear();
    //time.clear();
    statistics.clear();
    statistics.push_back(MAX_INIT);
    statistics.push_back(MIN_INIT);
    statistics.push_back(SUM_INIT);
    statistics.push_back(AVG_INIT);
    statistics.push_back(SD_INIT);
    ids.clear();
    AddId(id);
}

template <class T>
std::vector<std::string> TemporalData<T>::GetTime() const
{
    return time;
}

template <class T>
std::vector<T> TemporalData<T>::GetData() const
{
    return data;
}

template <class T>
std::vector<int>  TemporalData<T>::GetIds() const
{
    return ids;
}

template <class T>
int TemporalData<T>::GetSize()  const
{
    return data.size();
}

template <class T>
double TemporalData<T>::GetStatistic(Statistic statistic)  const
{
    Q_ASSERT((int) statistics.size() > statistic);
    return statistics[statistic];
}

template <class T>
std::vector<double /*T*/ > TemporalData<T>::GetStatistics() const
{
    return statistics;
}

template <class T>
void TemporalData<T>::SetData(int id, const std::vector<T>& timeSeriesData)
{
    Init(id);
    data.assign(timeSeriesData.begin(), timeSeriesData.end());
    ComputeStatistics();
}

/*!
    Creates a time-value mapping for the temporal data
*/
template <class T>
void TemporalData<T>::SetTime(const std::vector<std::string>& timeSeriesTime)
{
    time.clear();
    time.assign(timeSeriesTime.begin(), timeSeriesTime.end());
}


template <class T>
void TemporalData<T>::SetStatistics(const std::vector<double /*T*/ >& stat)
{
    statistics.clear();
    statistics.assign(stat.begin(), stat.end());
}

template <class T>
void TemporalData<T>::SetStatistic(Statistic statistic, double /*T*/ value)
{
    Q_ASSERT((int) statistics.size() > statistic);
    statistics[statistic] = value;
}

/*!
    Add more data to a temporal data store (used when merging bins)
*/
template <class T>
void TemporalData<T>::AddData(const TemporalData<T>& bin)
{
    double avg, sd, n1, n2;

    std::vector<T> timeSeriesData = bin.GetData();
    std::vector<std::string> timeSeriesTime = bin.GetTime();
    std::vector<double> statistics = bin.GetStatistics();

    n1 = data.size();
    n2 = timeSeriesData.size();

    std::copy(timeSeriesData.begin(), timeSeriesData.end(), std::back_inserter(data));
    std::copy(timeSeriesTime.begin(), timeSeriesTime.end(), std::back_inserter(time));

    // ToDo: Remove this
    // ComputeStatistics();

    // Don't recompute the statistics, use the algebric property of the measures
    this->statistics[MAX] = (this->statistics[MAX] > statistics[MAX]) ? this->statistics[MAX] : statistics[MAX];
    this->statistics[MIN] = (this->statistics[MIN] < statistics[MIN]) ? this->statistics[MIN] : statistics[MIN];
    this->statistics[SUM] += statistics[SUM];

    Q_ASSERT(data.size() > 0);
    Q_ASSERT(data.size() == (n1 + n2));

    avg = this->statistics[SUM] / data.size();
    sd = sqrt((n1 * this->statistics[SD] * this->statistics[SD] + n2 * bin.GetStatistic(SD) * bin.GetStatistic(SD)
               + n1 * this->statistics[AVG] * this->statistics[AVG] + n2 * bin.GetStatistic(AVG) * bin.GetStatistic(AVG)) / (n1 + n2) - avg * avg);

    this->statistics[AVG] = avg;
    this->statistics[SD] = sd;
 }

template <class T>
void TemporalData<T>::AddId(int id)
{
    ids.push_back(id);
}

/*!
    Computes the statistics from the data
*/
template <class T>
void TemporalData<T>::ComputeStatistics()
{
    Q_ASSERT(statistics.size() >= AVG);

    T max = MAX_INIT, min = MIN_INIT, sum = SUM_INIT;
    double sumsq = SD_INIT * SD_INIT;

    for (typename std::vector<T>::iterator it = data.begin(); it != data.end(); ++it)
    {
        if (max < (*it))
        {
            max = (*it);
        }

        if (min > (*it))
        {
            min = (*it);
        }

        sum += (*it);
        sumsq += (*it) * (*it);
    }

    statistics[MAX] = max;
    statistics[MIN] = min;
    statistics[SUM] = sum;

    Q_ASSERT(data.size() > 0);
    statistics[AVG] = (sum * 1.0) / data.size();
    statistics[SD] = sqrt((sumsq * 1.0) / data.size() - statistics[AVG] * statistics[AVG]);
}

/*!
    Save data to a CSV file
*/
template <class T>
void TemporalData<T>::Serialize(const char *fileName, FileFormat format)
{
    Q_ASSERT(fileName != "");
    std::ofstream outFile(fileName);

    switch (format)
    {
        case CSV:
            /* outFile << "Max" << ',' << "Min" << ',' << "Sum" << ',' << "Avg" << ',' << std::endl;
            for (int i = 0; i < (int) statistics.size(); ++i)
            {
                outFile << statistics[i];
                if (i < (int) statistics.size() - 1)
                {
                    outFile << ',';
                }
                else
                {
                    outFile << std::endl;
                }
            }
            outFile << ",,,,,Data,,,,," << std::endl; */
            for (int i = 1; i <= (int) data.size(); ++i)
            {
                outFile << data[i - 1];
                if (!(i % 10))
                {
                    outFile << std::endl;
                }
                else if (i < (int) data.size())
                {
                    outFile << ",";
                }
            }
            break;

        case XML:

            break;

        default:

            break;
    }
}

/*!
    Read a CSV file to load data
*/
template <class T>
void TemporalData<T>::Deserialize(const char *fileName, FileFormat format)
{
    Q_ASSERT(fileName != "");

    std::ifstream inFile(fileName);
    char buffer[MAX_BUF];
    char *pData = 0;
    Statistic s = MAX;

    switch (format)
    {
        case CSV:            

            /*inFile.getline(buffer, MAX_BUF);

            inFile.getline(buffer, MAX_BUF);
            pData = strtok (buffer," ,\n");
            while (pData != NULL)
            {
                statistics[s] = atoi(pData);
                pData = strtok (NULL, " ,\n");
            }

            inFile.getline(buffer, MAX_BUF);*/

            data.clear();
            inFile.getline(buffer, MAX_BUF);
            while (inFile)
            {
                pData = strtok (buffer," ,\n");
                while (pData != NULL)
                {                    
                    data.push_back(atoi(pData));
                    pData = strtok (NULL, " ,\n");
                }
                inFile.getline(buffer, MAX_BUF);
            }

            ComputeStatistics();

            break;

        case XML:

            break;

        default:

            break;
    }
}

template <class T>
void TemporalData<T>::Clear()
{
    data.clear();
}

template TemporalData<int>::TemporalData(int);
template TemporalData<int>::TemporalData(int, std::vector<int>::const_iterator, std::vector<int>::const_iterator);
//template TemporalData<int>::TemporalData(const TemporalData<int>&);
//template TemporalData<int>& TemporalData<int>::operator=(const TemporalData<int>&);

template std::vector<std::string> TemporalData<int>::GetTime() const;
template std::vector<int> TemporalData<int>::GetData() const;
template int TemporalData<int>::GetSize() const;
template double TemporalData<int>::GetStatistic(Statistic) const;

template void TemporalData<int>::SetTime(const std::vector<std::string>&);
template void TemporalData<int>::SetData(int, const std::vector<int>&);
template void TemporalData<int>::SetStatistics(const std::vector<double>&);
template void TemporalData<int>::SetStatistic(Statistic, double);
template void TemporalData<int>::AddData(const TemporalData<int>&);

template void TemporalData<int>::ComputeStatistics();

//template void TemporalData<int>::Serialize(const char*, FileFormat);
//template void TemporalData<int>::Deserialize(const char*, FileFormat);

template void TemporalData<int>::Clear();

template TemporalData<double>::TemporalData(int);
template TemporalData<double>::TemporalData(int, std::vector<double>::const_iterator, std::vector<double>::const_iterator);
//template TemporalData<double>::TemporalData(const TemporalData<double>&);
//template TemporalData<double>& TemporalData<double>::operator=(const TemporalData<double>&);

template std::vector<std::string> TemporalData<double>::GetTime() const;
template std::vector<double> TemporalData<double>::GetData() const;
template int TemporalData<double>::GetSize() const;
template double TemporalData<double>::GetStatistic(Statistic) const;

template void TemporalData<double>::SetTime(const std::vector<std::string>&);
template void TemporalData<double>::SetData(int, const std::vector<double>&);
template void TemporalData<double>::SetStatistics(const std::vector<double>&);
template void TemporalData<double>::SetStatistic(Statistic, double);
template void TemporalData<double>::AddData(const TemporalData<double>&);

template void TemporalData<double>::ComputeStatistics();

//template void TemporalData<double>::Serialize(const char*, FileFormat);
//template void TemporalData<double>::Deserialize(const char*, FileFormat);

template void TemporalData<double>::Clear();
