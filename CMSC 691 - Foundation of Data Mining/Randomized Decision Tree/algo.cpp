#include <fstream>
#include <QMessageBox>
#include "algo.h"

#define MAX_LINE 4096
#define NUM_BIN_CONT 3 //5
#define TRAIN_FRACTION //0.25
#define TEST_FRACTION 0.2 //0.3

/*! Constructor */
Algo::Algo() //:RDT(0)
{
}

std::vector<std::string> Algo::GenAllTuples()
{
    int n = features.size();
    std::vector<std::string> v;
    if (!n)
    {
        return v;
    }
    std::ofstream f("temp5.txt");
    std::vector<std::string> range;
    for (int i = 0; i < n; ++i)
    {
        std::vector<std::string> tv;
        if (featurevalues.find(features[i]) == featurevalues.end())
        {
            continue;
        }
        range = featurevalues[features[i]];
        int ns = v.size();
        if (!ns)
        {
            for (int k = 0; k < range.size(); ++k)
            {
                tv.push_back(range[k]);
            }
        }
        for (int j = 0; j < ns; ++j)
        {
            std::string current = v[j];
            for (int k = 0; k < range.size(); ++k)
            {
                tv.push_back(current + "," + range[k]);
            }
        }
        v.clear();
        v.assign(tv.begin(), tv.end());
    }
    for (std::map<std::string, std::vector<std::string> >::const_iterator it = featurevalues.begin(); it != featurevalues.end(); ++it)
    {
        f << it->first << ": ";
        std::copy(it->second.begin(), it->second.end(), std::ostream_iterator<std::string>(f, ", "));
        f << std::endl;
    }
    f << std::endl;
    std::copy(v.begin(), v.end(), std::ostream_iterator<std::string>(f, "\n"));
    return v;
}

double Algo::LoadDataAndTrainTest(const std::string& filename, int N, int k, bool distributed, double factor)
{
    Initialize(filename);
    ReadData(distributed, N, factor);
    ReadNames();
    BuildRDTs(N, k);
    TrainRDTs(distributed);
    BuildFourierCoefficientVector();
    double accuracy = TestRDTs();
    /* */
    Cleanup();
    w.clear();
    /* */
    return accuracy;
}

double Algo::LoadDataAndTrainWeightedTest(const std::string& filename, int N, int k, int numiter, bool distributed, double factor)
{
    Initialize(filename);
    ReadData(distributed, N, factor);
    ReadNames();
    BuildRDTs(N, k);
    TrainRDTs(distributed);
    std::vector<std::string> lines;
    char buf[MAX_LINE];
    std::ifstream f;
    f.open((dataFile + ".test").c_str());
    while (f)
    {
        f.getline(buf, MAX_LINE);
        lines.push_back(buf);
    }
    f.close();
    RandomSample(lines, numiter, factor, false);
    double accuracy;
    std::ofstream of("temp3.txt"), of1("temp2.txt");
    for (int i = 0; i < numiter; ++i)
    {
        std::copy(w.begin(), w.end(), std::ostream_iterator<double>(of, " "));
        sprintf(buf, "%s_%d.test", dataFile.c_str(), i + 1);
        f.open(buf);
        accuracy = Test(f, of1);
        f.close();
        of << std::endl;
        std::copy(acc.begin(), acc.end(), std::ostream_iterator<double>(of, " "));
        of << std::endl;
        of << accavg << std::endl;
    }
    /* */
    Cleanup();
    /* */
    return accuracy;
}

void Algo::Initialize(const std::string& filename)
{
    Cleanup();
    dataFile = filename;
}

void Algo::Cleanup()
{
    status = "";
    classes.clear();
    features.clear();
    featurevalues.clear();
    for (int i = 0; i < RDT.size(); ++i)
    {
        DeleteTreeStructure(RDT[i]);
        RDT[i] = NULL;
    }
    RDT.clear();
    acc.clear();
    accavg = 0;
    /*if (dataFile != "")
    {
        dataFile += "_*.data";
        remove(dataFile.c_str());
        dataFile = "";
    }*/
}

void Algo::ReadData(bool distributed, int N, double factor)
{
    status = "Reading Data";
    std::vector<std::string> lines;
    char buf[MAX_LINE];
    //memcpy(buf, 0, MAX_LINE);
    int i = 0, nt = 1;
    std::ifstream f;
    f.open((dataFile + ".data").c_str());
    f.getline(buf, MAX_LINE);
    if (distributed)
    {
        lines.push_back(buf);
    }
    char *p = strtok(buf, ", \t");
    while (p)
    {
            max.push_back(atof(p));
            min.push_back(atof(p));
            ++i;
            p = strtok(NULL, ", \t");
    }
    while (f)                       // read feature values
    {
            i = 0;
            f.getline(buf, MAX_LINE);
            if (distributed)
            {
                lines.push_back(buf);
            }
            p = strtok(buf, ", \t");
            while (p)
            {
                    double val = atof(p);
                    if (max[i] < val)
                    {
                        max[i] = val;
                    }
                    if (min[i] > val)
                    {
                        min[i] = val;
                    }
                    p = strtok(NULL, ", \t");
                    ++i;
            }
            ++nt;
    }
    f.close();
    NT = nt;

    if (distributed)
    {
        RandomSample(lines, N, factor);
    }
}

void Algo::RandomSample(const std::vector<std::string>& datavector, int N, double factor, bool train)
{
    char file[MAX_LINE];
    std::ofstream f;
    for (int i = 0; i < N; ++i)
    {
        if (train)
        {
            sprintf(file, "%s_%d.data", dataFile.c_str(), i + 1);
        }
        else
        {
            sprintf(file, "%s_%d.test", dataFile.c_str(), i + 1);
        }
        f.open(file);
        int n = datavector.size();
        int j = 0, nt = (int)((factor * n) / N); //TRAIN_FRACTION * NT;
        while (j < nt)                       // randomly select data
        {
            f << datavector[rand() % n] << std::endl;
            ++j;
        }
        f.close();
    }
}

void Algo::ReadNames()
{
    status = "Reading Names";
    char buf[MAX_LINE];
    std::ifstream f;
    f.open((dataFile + ".names").c_str());
    f.getline(buf, MAX_LINE);       // read class label
    char *p = strtok(buf, ",. \t");
    while (p)
    {
            classes.push_back(p);
            p = strtok(NULL, ",. \t");
    }

    int i = 0;
    while (f)               // read features
    {
            f.getline(buf, MAX_LINE);
            char *name = strtok(buf, ":,. \t");
            if (name)
            {
                features.push_back(name);
                p = strtok(NULL, ":,. \t");
                if (p)
                {
                    //if (strcmp(strupr(p), "IGNORE"))
                    if (strcmp(p, "ignore"))
                    {
                        std::vector<std::string> val;
                        //if (!strcmp(strupr(p), "CONTINUOUS"))
                        if (!strcmp(p, "continuous"))
                        {
                            double binsz = (max[i] - min[i]) / NUM_BIN_CONT, start = min[i];
                            char s[64];
                            for (int i = 0; i < NUM_BIN_CONT; ++i)
                            {
                                sprintf(s, "[%lf %lf]", start, start + binsz);
                                start += binsz;
                                val.push_back(s);
                            }
                        }
                        else
                        {
                            while (p)
                            {
                                val.push_back(p);
                                p = strtok(NULL, ":,. \t");
                            }
                        }
                        featurevalues[name] = val;
                    }
                }
                ++i;
            }
    }
    f.close();
}

void Algo::BuildRDTs(int N, int k)
{
    status = "Generating RDTs";
    int n = featurevalues.size();
    if (k > n)
    {
        k = n;
    }
    std::vector<std::string> v(n); //, randfeatures;
    std::map<std::string, std::vector<std::string> >::iterator it = featurevalues.begin();
    for (int i = 0; i < n; ++i, ++it)
    {
        v[i] = it->first;
    }
    for (int i = 0; i < N; ++i)
    {
        node *t = NULL;
        RDT.push_back(BuildTreeStructure(t, k, 0, v));
        w.push_back(1.0 / N);   // uniform weights
        acc.push_back(0);
    }    
}

node* Algo::BuildTreeStructure(node *n, int k, int level, const std::vector<std::string>& randfeatures)
{
    if (!n)
    {
       n = new node;
       for (int i = 0; i < classes.size(); ++i)
       {
           n->classdist[classes[i]] = 0;
       }
    }
    int numfeatures = randfeatures.size();
    if (level < k) // not a leaf node
    {
       int sel = rand() % numfeatures;
       n->feature = randfeatures[sel];
       std::vector<std::string> nextfeatures;
       nextfeatures.assign(randfeatures.begin(), randfeatures.end());
       nextfeatures.erase(nextfeatures.begin() + sel);
       for (int i = 0; i < featurevalues[n->feature].size(); ++i)
       {
            n->next.push_back(NULL);
            n->next[i] = BuildTreeStructure(n->next[i], k, level + 1, nextfeatures);
       }
    }
    return n;
}

void Algo::DeleteTreeStructure(node *n)
{
    if (n)
    {
        for (int i = 0; i < n->next.size(); ++i)
        {
            DeleteTreeStructure(n->next[i]);
        }
        n->next.clear();
        n->classdist.clear();
    }
}

/*!
    Pre-order traversal of RDT
    Input - root of the tree
*/
void Algo::PreOrder(node *n, std::ofstream& f, std::string path)
{
    if (n)
    {
        if (n->next.size() == 0)
        {
            f << path << ": ";
            for (int i = 0; i < classes.size(); ++i)
            {
                f << " " << n->classdist[classes[i]];
            }
            f << std::endl;
        }
        //f << std::endl << "(" << n->feature << ")";
        std::vector<std::string> range = featurevalues[n->feature];
        path += n->feature + "=";
        for (int i = 0; i < n->next.size(); ++i)
        {
            //f << range[i];
            std::string nwpath = path + range[i] + " ";
            PreOrder(n->next[i], f, nwpath);
        }
    }
}

void Algo::TrainRDTs(bool distributed)
{
    status = "Training RDTs";
    std::ifstream f;
    int N = RDT.size();
    if (distributed)
    {
        char file[MAX_LINE];
        for (int i = 0; i < N; ++i) // train with samples
        {
            sprintf(file, "%s_%d.data", dataFile.c_str(), i + 1);
            f.open(file);
            Train(f, RDT[i], distributed);
            f.close();
        }
    }
    else
    {
        f.open((dataFile + ".data").c_str());   // train with population
        Train(f, 0, distributed);
        f.close();
    }
    std::ofstream outf("temp1.txt");
    for (int i = 0; i < N; ++i)
    {
        outf << std::endl << "RDT " << i + 1 << ":" << std::endl;
        PreOrder(RDT[i], outf, "");
    }
    outf.close();
}

void Algo::Train(std::ifstream& f, node *nRDT, bool distributed)
{
    char buf[MAX_LINE];
    int n = features.size();
    int N = RDT.size();
    std::ofstream of("temp.txt");
    while (f)                       // read feature values
    {
            std::map<std::string, std::string> fval;
            std::map<std::string, std::string>::const_iterator it;
            int i = 0;
            f.getline(buf, MAX_LINE);
            char *p = strtok(buf, ", \t");
            while (p && i < n)
            {
                    if (featurevalues.find(features[i]) != featurevalues.end())
                    {
                        fval[features[i]] = p;
                    }
                    p = strtok(NULL, ", \t");
                    ++i;
            }
            if (p)
            {
                std::string c(p);
                if (distributed)
                {
                    UpdateStatistics(nRDT, fval, c);
                }
                else
                {
                    for (int j = 0; j < N; ++j)       // Wei Fan
                    {
                        UpdateStatistics(RDT[j], fval, c);
                    }
                }
            }
            for (it = fval.begin(); it != fval.end(); ++it)
            {
                of << (*it).first << ": " << (*it).second << " ";
            }
            of << p << std::endl;
    }
    f.close();
}

void Algo::UpdateStatistics(node* n, std::map<std::string, std::string>& fval, std::string& c)
{
    ++n->classdist[c];
    const char *ct = c.c_str();
    //if (n->feature != "")
    if (n && n->next.size())
    {
        std::string fv = fval[n->feature];
        std::vector<std::string> range = featurevalues[n->feature];
        double v = atof(fv.c_str());
        int nc = 0;
        for (int i = 0; i < range.size(); ++i)
        {
            if (range[i][0] == '[')
            {
                char *p = strtok((char*)range[i].c_str(), "[ ]");
                double l = atof(p);
                p = strtok(NULL, "[ ]");
                double u = atof(p);
                if (l <= v && v <= u)
                {
                    nc = i;
                    break;
                }
            }
            else if (range[i] == fv)
            {
                nc = i;
                break;
            }
        }
        /*if (fv == "?")
        {
            nc = 0;
        }*/
        UpdateStatistics(n->next[nc], fval, c);
    }
}

double Algo::TestRDTs()
{
    status = "Testing RDTs";
    std::ofstream outf("temp2.txt");
    std::ifstream f((dataFile + ".test").c_str());
    double accuracy = Test(f, outf);
    /*QMessageBox::warning(0, "Accuracy",
                         QString::number(accuracy),
                         QMessageBox::Ok,
                         QMessageBox::Ok);*/
    f.close();
    outf.close();
    status = "";
    return accuracy;
}

double Algo::Test(std::ifstream& f, std::ofstream& of)
{
    char buf[MAX_LINE];
    int n = features.size();
    int correct = 0, total = 0;
    int N = RDT.size();
    std::vector<int> cor(N);
    double max;
    while (f)                       // read feature values
    {
            std::map<std::string, std::string> fval;
            std::map<std::string, std::string>::const_iterator it;
            int i = 0;
            f.getline(buf, MAX_LINE);
            char *p = strtok(buf, ", \t");
            while (p && i < n)
            {
                if (featurevalues.find(features[i]) != featurevalues.end())
                {
                    fval[features[i]] = p;
                }
                p = strtok(NULL, ", \t");
                ++i;
            }

            if (p)
            {
                std::string actual = p;

                std::map<std::string, double> avgdist;
                for (int j = 0; j < classes.size(); ++j)
                {
                    avgdist[classes[j]] = 0;
                }
                for (it = fval.begin(); it != fval.end(); ++it)
                {
                    of << (*it).first << "=" << (*it).second << " ";
                }
                of << std::endl;
                for (int r = 0; r < N; ++r)
                {
                    std::map<std::string, int> dist = Classify(RDT[r], fval);
                    of << "Tree " << r + 1 << "=> ";
                    for (int j = 0; j < classes.size(); ++j)
                    {
                        of << classes[j] << ": " << dist[classes[j]] << " ";
                    }
                    max = dist[classes[0]];
                    std::string target = classes[0];
                    for (int j = 1; j < classes.size(); ++j)
                    {
                        if (max < dist[classes[j]])
                        {
                            max = dist[classes[j]];
                            target = classes[j];
                        }
                    }
                    of << " Class: " << target << std::endl;
                    if (actual == target)
                    {
                        ++cor[r];
                    }
                    int sum = 0;
                    for (int j = 0; j < classes.size(); ++j)
                    {
                        sum += dist[classes[j]];
                    }
                    for (int j = 0; j < classes.size(); ++j)
                    {
                        avgdist[classes[j]] += w[j] * (sum ? (dist[classes[j]] * 1.0) / sum : 1.0 / classes.size());
                    }
                }
                of << "Ensemble => ";
                for (int j = 0; j < classes.size(); ++j)
                {
                    //avgdist[classes[j]] /= N;
                    of << classes[j] << ": " << avgdist[classes[j]] << " ";
                }
                of << std::endl;
                max = avgdist[classes[0]];
                std::string target = classes[0];
                for (int i = 1; i < classes.size(); ++i)
                {
                    if (max < avgdist[classes[i]])
                    {
                        max = avgdist[classes[i]];
                        target = classes[i];
                    }
                }
                if (actual == target)
                {
                    ++correct;
                }
                of << "Actual: " << actual << std::endl;
                of << "Ensemble: " << target << std::endl;
            }
            ++total;
    }
    f.close();
    int totcor = 0;
    for (int r = 0; r < N; ++r)
    {
        acc[r] = cor[r] * 100.0 / total;
        of << acc[r] << std::endl;
        totcor += cor[r];
    }
    of << correct * 100.0 / total << std::endl;
    of << "New Weights: ";
    for (int r = 0; r < N; ++r)
    {
        w[r] = (cor[r] * 1.0) / totcor;
        of << w[r] << " ";
    }
    of << std::endl;
    accavg = correct * 100.0 / total;
    return accavg;
}

// match the test tuple with a path in the RDT
std::map<std::string, int>& Algo::Classify(node* n, std::map<std::string, std::string>& fval)
{
    if (n && n->next.size())
    {
        std::string fv = fval[n->feature];
        std::vector<std::string> range = featurevalues[n->feature];
        int nc = 0;
        for (int i = 0; i < range.size(); ++i)
        {
            if (range[i][0] == '[')
            {
                char *p = strtok((char*)range[i].c_str(), "[ ]");
                double l = atof(p);
                p = strtok(NULL, "[ ]");
                double u = atof(p);
                double v = atof(fv.c_str());
                if (l <= v && v <= u)
                {
                    nc = i;
                    break;
                }
            }
            else if (range[i] == fv)
            {
                nc = i;
                break;
            }
        }
        /*if (fv == "?")
        {
            nc = 0;
        }*/
        return Classify(n->next[nc], fval);
    }
    else
    {
        return n->classdist;
    }
}

// match the test tuple with a path in the RDT
std::map<std::string, int>& Algo::Match(node* n, std::map<std::string, std::string>& fval)
{
    if (n && n->next.size())
    {
        std::string fv = fval[n->feature];
        std::vector<std::string> range = featurevalues[n->feature];
        int nc = 0;
        for (int i = 0; i < range.size(); ++i)
        {
            if (range[i] == fv)
            {
                nc = i;
                break;
            }
        }
        return Match(n->next[nc], fval);
    }
    else
    {
        return n->classdist;
    }
}

int innerprod(std::string& j, std::string& x)
{
    int n = j.length();
    int prod = 0;
    for (int i = 0; i < n; ++i)
    {
        prod += (j[i] - '0') * (x[i] - '0');
    }
    return prod % 2;
}

void Algo::BuildFourierCoefficientVector()
{
    std::ofstream ouf("temp4.txt");
    std::vector<std::string> vtuples = GenAllTuples();
    for (int j = 0; j < RDT.size(); ++j)
    {
        ouf << "RDT " << j << ": " << std::endl;
        for (int i = 0; i < vtuples.size(); ++i)
        {
            std::map<std::string, std::string> fval;
            char *p = strtok((char*)vtuples[i].c_str(), ",");
            int i = 0;
            while (p)
            {
                fval[features[i++]] = p;
                p = strtok(NULL, ",");
            }
            std::map<std::string, int> cd = Match(RDT[j], fval);
            std::map<std::string, int>::const_iterator it;
            int sum = 0;
            for (it = cd.begin(); it != cd.end(); ++it)
            {
                sum += it->second;
            }
            for (it = cd.begin(); it != cd.end(); ++it)
            {
                ouf << (sum ? (it->second * 100.0) / sum : 100.0 / classes.size()) << ", ";
            }
            ouf << std::endl;
        }
        ouf << std::endl;
    }
    ouf << "RDT Fourier " << std::endl;
    for (int r = 0; r < RDT.size(); ++r)
    {
        ouf << "RDT " << r << ": " << std::endl;
        for (int j = 0; j < vtuples.size(); ++j)
        {
            double wj = 0;
            for (int x = 0; x < vtuples.size(); ++x)
            {
                std::map<std::string, std::string> fval;
                char *p = strtok((char*)vtuples[x].c_str(), ",");
                int i = 0;
                while (p)
                {
                    fval[features[i++]] = p;
                    p = strtok(NULL, ",");
                }
                std::map<std::string, int> cd = Match(RDT[r], fval);
                std::map<std::string, int>::const_iterator it;
                int sum = 0;
                for (it = cd.begin(); it != cd.end(); ++it)
                {
                    sum += it->second;
                }
                for (it = cd.begin(); it != cd.end(); ++it)
                {
                    wj += (sum ? (it->second * 100.0) / sum : 100.0 / classes.size())
                          * (innerprod(vtuples[j], vtuples[x]) ? -1 : 1);
                }
            }
            ouf << wj << std::endl;
        }
        ouf << std::endl;
    }
}

void Algo::DrawTree(QPainter& p, node *n, int x, int y, int g)
{
    static int xC = 10;
    if (n)
    {
        g /= 1.5;
        p.drawEllipse(xC + x, y, 5, 5);
        if (n->feature == "" && n->classdist.size())
        {
            int max = n->classdist[classes[0]];
            char *ds = (char*)classes[0].c_str();
            for (int i = 1; i < classes.size(); ++i)
            {
                if (max < n->classdist[classes[i]])
                {
                    max = n->classdist[classes[i]];
                    ds = (char*)classes[i].c_str();
                }
            }
            p.drawText(xC + x, y + 20, ds);
        }
        else
        {
            p.drawText(xC + x + 2, y - 10, n->feature.c_str());
        }
        int nc = n->next.size(), xc = x - g * nc / 2;
        for (int i = 0; i < nc; ++i, xc += g)
        {
            p.drawLine(xC + x, y, xC + xc, y + 100);
            //p.drawText(xC + (x + xc) / 2, y + 60, featurevalues[n->feature][i].c_str());
            DrawTree(p, n->next[i], xc, y + 100, g);
        }
    }
}

void Algo::PreProcess(const std::string& file)
{
    status = "Preprocessing";
    std::vector<std::string> dv;
    std::ifstream inf(file.c_str());
    std::ofstream f;
    char buf[MAX_LINE];
    while (inf)
    {
        inf.getline(buf, MAX_LINE);
        dv.push_back(buf);
    }
    int np = dv.size(), ns = TEST_FRACTION * np;
    f.open((file + ".test").c_str());
    for (int i = 0; i < ns; ++i)
    {
        int sel = rand() % np;
        f << dv[sel] << std::endl;
        dv.erase(dv.begin() + sel);
        --np;
    }
    f.close();
    f.open((file + ".data").c_str());
    std::copy(dv.begin(), dv.end(), std::ostream_iterator<std::string>(f, "\n"));
    f.close();
    status = "Done";
}

void Algo::PreProcessEARDMS()
{
    char buf[MAX_LINE];
    std::ifstream if1("data\\nsf_EAR"), if2("data\\nsf_DMS");
    std::ofstream of("data\\nsf");
    while (true)
    {
        int r = rand() % 8;
        if (r >= 4 && if1)
        {
            if1.getline(buf, MAX_LINE);
        }
        else if (if2)
        {
            if2.getline(buf, MAX_LINE);
        }
        else
        {
            break;
        }
        of << buf << std::endl;
    }
}

void Algo::PreProcessBinary(const std::string& file)
{
    char buf[MAX_LINE];
    //memcpy(buf, 0, MAX_LINE);
    int i = 0, nt = 1;
    std::ifstream f;
    f.open(file.c_str());
    f.getline(buf, MAX_LINE);
    char *p = strtok(buf, ", \t");
    while (p)
    {
            max.push_back(atof(p));
            min.push_back(atof(p));
            ++i;
            p = strtok(NULL, ", \t");
    }
    while (f)                       // read feature values
    {
            i = 0;
            f.getline(buf, MAX_LINE);
            p = strtok(buf, ", \t");
            while (p)
            {
                    double val = atof(p);
                    if (max[i] < val)
                    {
                        max[i] = val;
                    }
                    if (min[i] > val)
                    {
                        min[i] = val;
                    }
                    p = strtok(NULL, ", \t");
                    ++i;
            }
    }
    f.close();
    std::ofstream of((file + "_bin").c_str());
    f.open(file.c_str());
    while (f)                       // read feature values
    {
            i = 0;
            f.getline(buf, MAX_LINE);
            p = strtok(buf, ", \t");
            while (p)
            {
                of << ((atof(p) / max[i]) > 0.5 ? 1 : 0) << " ";
                p = strtok(NULL, ", \t");
                ++i;
            }
            of << std::endl;
    }
    Cleanup();
}

void Algo::RunTests(const std::string& inf)
{
    /* *\/
    std::ofstream f("result.csv");
    int kstart = 6, kmax = 6, kstep = 1;
    int Nstart = 10, Nmax = 100, Nstep = 20, Ntrial = 2;
    f << "k, ";
    for (int N = Nstart; N <= Nmax; N += Nstep)
    {
        f << N << ",";
    }
    f << std::endl;
    for (int k = kstart; k <= kmax; k += kstep)
    {
        std::vector<double> av, sv;
        f << "RDT" << "," << k << ",";
        for (int N = Nstart; N <= Nmax; N += Nstep)
        {
            double avg = 0, savg = 0, cur = 0;
            for (int i = 0; i < Ntrial; ++i)
            {
                cur = LoadDataAndTrainTest(inf, N, k, false);
                avg += cur;
                savg += cur * cur;
            }
            //f << avg / Ntrial << "," << savg / Ntrial - (avg / Ntrial) * (avg / Ntrial) << ",";
            av.push_back(avg / Ntrial);
            sv.push_back(savg / Ntrial - (avg / Ntrial) * (avg / Ntrial));
        }
        std::copy(av.begin(), av.end(), std::ostream_iterator<double>(f, ","));
        std::copy(sv.begin(), sv.end(), std::ostream_iterator<double>(f, ","));
        f << std::endl;
        av.clear();
        sv.clear();
        f << "DRDT" <<"," << k << ",";
        for (int N = Nstart; N <= Nmax; N += Nstep)
        {
            double avg = 0, savg = 0, cur = 0;
            for (int i = 0; i < Ntrial; ++i)
            {
                cur = LoadDataAndTrainTest(inf, N, k, true);
                avg += cur;
                savg += cur * cur;
            }
            //f << avg / Ntrial << "," << savg / Ntrial - (avg / Ntrial) * (avg / Ntrial) << ",";
            av.push_back(avg / Ntrial);
            sv.push_back(savg / Ntrial - (avg / Ntrial) * (avg / Ntrial));
        }
        std::copy(av.begin(), av.end(), std::ostream_iterator<double>(f, ","));
        std::copy(sv.begin(), sv.end(), std::ostream_iterator<double>(f, ","));
        f << std::endl;
        QMessageBox::warning(0, "Done",
                                 QString::number(k),
                                 QMessageBox::Ok,
                                 QMessageBox::Ok);
    }
    /*  */

    /*  *\/
    int k = 5, N = 25;
    f << "DRDT" << std::endl;
    for (double fc = 1.0; fc <= 10.0; fc += 0.5)
    {
        f << LoadDataAndTrainTest(inf, N, k, true, fc) << std::endl;
    }
    QMessageBox::warning(0,  "Done",
                             "Done",
                             QMessageBox::Ok,
                             QMessageBox::Ok);
    /* */

    /* */
    int k = 2, N = 2;
    QMessageBox::warning(0, "Done",
                             QString::number(LoadDataAndTrainTest(inf, N, k, true, 2)),
                             QMessageBox::Ok,
                             QMessageBox::Ok);
    /* */
}
