#include<cmath>
#include<stdlib.h>
#include<iostream>
#include "utils.h"



const double kPrecision = 0.0000000001;

void Utils::splitStrings(string& line,char delimiter,vector<string>* r)
{
    int begin = 0;
    for (int i = 0; i < line.size(); i++)
    {
        if(line[i] == delimiter)
        {
            (*r).push_back(line.substr(begin,i - begin));
            begin = i + 1;
        }
    }
    if(begin < line.size())
    {
        (*r).push_back(line.substr(begin,line.size()-begin));
    }
    
}

int Utils::sgn(double x)
{
    if(x > kPrecision)
        return 1;
    else
        return -1;
}

double Utils::uniform()
{
    return rand()/((double)RAND_MAX +1.0);
}

double Utils::gaussian()
{
    double u,v,x,y,Q;
    do
    {
        do
        {
            u = uniform();
        } while (u == 0.0);
        v = 1.7156 * (uniform() - 0.5);
        x = u - 0.449871;
         y = abs(v) + 0.386595;
         Q = x * x + y * (0.19600 * y - 0.25472 * x);
    }while (Q >= 0.27597 && (Q > 0.27846 || v * v > -4.0 * u * u * log(u)));
    return v/u;
}

double Utils::gaussian(double mean,double stdev)
{
    if (stdev ==0.0)
    {
        return mean;
    }
    else
    {
        return mean + stdev*uniform();
    }
    
}

int main(int argc, char const *argv[])
{
    double mean = 0.0;
    double stdev = 1;
    // 调用类的静态方法
    double res = Utils::gaussian(mean,stdev);

    std::cout <<  res << endl;

    return 0;
}
