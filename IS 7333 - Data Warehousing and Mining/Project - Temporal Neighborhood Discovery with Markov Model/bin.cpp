#include "bin.h"

Bin::Bin() : deleted(false)
{
}

Bin::Bin(int sz, const std::vector<double> stat)
{
    setSize(sz);
    setStatistics(stat);
}

int Bin::getSize()  const
{
    return size;
}

double Bin::getStatistic(Statistic statistic)  const
{
    Q_ASSERT((int) statistics.size() > statistic);
    return statistics[statistic];
}

std::vector<double> Bin::getStatistics() const
{
    return statistics;
}

bool Bin::deleted() const
{
    return deleted;
}

void Bin::setSize(int sz)
{
    size = sz;
}

void Bin::setStatistics(const std::vector<double> stat)
{
    statistics.clear();
    statistics.assign(stat.begin(), stat.end());
}

void Bin::SetStatistic(Statistic statistic, double value)
{
    Q_ASSERT((int) statistics.size() > statistic);
    statistics[statistic] = value;
}

void Bin::setDeleted(bool del)
{
    deleted = del;
}
