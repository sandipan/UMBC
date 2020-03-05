#ifndef BIN_H
#define BIN_H

#include <vector>
#include "types.h"

class Bin
{
    int size;
    // aggregate statistics
    std::vector<double> statistics;
    bool deleted;

public:

    // constructor
    Bin();
    Bin(int, const std::vector<double>);

    // get methods
    int getSize() const;
    double getStatistic(Statistic) const;
    std::vector<double> getStatistics() const;

    bool deleted() const;

    // set methods
    void setSize(int);
    void setStatistics(const std::vector<double>);
    void setStatistic(Statistic, double);    

    void setDeleted(bool);
};

#endif // BIN_H
