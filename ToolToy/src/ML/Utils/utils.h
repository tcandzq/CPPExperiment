#ifndef TOOL_TOY_UTILS_H
#define TOOL_TOY_UTILS_H

#include<string>
#include<vector>

using namespace std;

class Utils
{
    public:
        void static splitStrings(string& line,char delimiter,vector<string>* r);

        int static sgn(double x);

        double static uniform();

        double static gaussian();

        double static gaussian(double mean,double stdev);
};


#endif