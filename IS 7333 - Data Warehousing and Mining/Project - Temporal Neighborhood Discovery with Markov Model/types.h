/* // ----------------------------------------------------------------
   //                   Types
   // ----------------------------------------------------------------
   //                   Sandipan Dey
   //                   Graduate Student
   //                   UMBC
   // ----------------------------------------------------------------*/

#ifndef TYPES_H
#define TYPES_H

// Enumerators
enum Statistic
{
    MAX,
    MIN,
    SUM,
    AVG,
    SD
};

enum Distribution
{
    None,
    Gaussian
};

enum Algorithm
{
    SimilarityWithTrMatrix,
    SimilarityWithMarkovStationary,
    DynamicProgramming,
    Greedy
    /*, SimilarityWithDFT */
};

enum DistanceMetric
{
    Bhattacharya,
    KL,
    Mahalanobis,
    Hellinger
    /*, KLSym */
};

enum FileFormat
{
    CSV,
    XML
};

enum NormalizationMetric
{
    SSD,
    SSRD,
    SMD
};

#endif // TYPES_H
