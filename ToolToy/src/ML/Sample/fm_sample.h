#ifndef FM_SAMPLE_H_
#define FM_SAMPLE_H_

#include<string>
#include<vector>
#include<iostream>

using namespace std;

const string spliter = " ";
const string innerSpliter = ":";




class fm_sample
{
    public:
        int y; // 标签
        vector<pair<string,double>> x; // 特征对集合
        fm_sample(const string& line);
};
// line:一行记录；class_num:标签数量
fm_sample::fm_sample(const string& line)
{
    this->x.clear();
    size_t posb = line.find_first_not_of(spliter,0);
    size_t pose = line.find_first_of(spliter,posb);
    int label = atoi(line.substr(posb,pose-posb).c_str());
    this->y = label > 0?1:-1; // 标签值
    string key; // 特征名称
    double value; // 特征值
    while (pose < line.size())
    {
        posb = line.find_first_not_of(spliter,pose);
        if(posb == string::npos) //查找字符串是否查找成功的标准做法
            break;
        pose = line.find_first_of(innerSpliter,posb);
        if(pose == string::npos)
        {
            cerr << "wrong line of sample input\n" << line << endl;
            exit(1);
        }
        key = line.substr(posb,pose-posb); // 获取特征名称
        posb = pose + 1;
        if(posb >= line.size())
        {
            cerr << "wrong line of sample input\n" << line << endl;
            exit(1);
        }
        pose = line.find_first_of(spliter,posb);
        value = stod(line.substr(posb,pose-posb));
        if(value != 0)
        {
            this->x.push_back(make_pair(key,value));
        }
    }
        
}

#endif