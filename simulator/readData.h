#ifndef _IOF_H_
#define _IOF_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <tuple>
#include <sys/stat.h>
#include <sys/types.h>
#include "macro.h"
#include "path.h"

std::vector<std::vector<std::pair<int,int>>> readLinkData(std::string filename) {
    int routerNum;
    std::ifstream infstream;
    infstream.open(filename, std::ios::in);
    infstream >> routerNum;
    std::vector<std::vector<std::pair<int,int>>> 
        res(routerNum, std::vector<std::pair<int,int>>(routerNum, std::pair<int,int>{-1,-1}));    
    int left, right, band, delay;
    while (!infstream.eof()){
        infstream >> left >> right >> band >> delay;
        res[left][right].first = band;
        res[left][right].second = delay;
    }
    infstream.close();
    return res;
};

std::vector<std::string> readNameData(std::string filename) {
    std::vector<std::string> res;
    std::ifstream infstream;
    infstream.open(filename, std::ios::in);
    std::string tmp;
    while (!infstream.eof()) {
        infstream >> tmp;
        res.emplace_back(tmp);
    }
    infstream.close();
    return res;
}

std::pair<std::vector<int>, std::vector<int>> readComData(std::string filename) {
    std::vector<int> sid, com;
    std::ifstream infstream;
    infstream.open(filename, std::ios::in);
    int SID, COM;
    while (!infstream.eof()) {
        infstream >> SID >> COM;
        sid.emplace_back(SID),
        com.emplace_back(COM);
    }
    infstream.close();
    return std::pair<std::vector<int>, std::vector<int>>{sid,com};
}

std::vector<double> split(std::string s) {
    std::vector<double> vec;
    if (s == "[]")
        return vec;
    int i = 0;
    std::string tmp;
    while (i < s.size()) {
        if (s[i] == ',' || s[i] == ']') {
            vec.emplace_back(stod(tmp));
            tmp.clear();
            ++i;
        }
        else if (s[i] == '[' || s[i] == ' ')
            ++i;
        else {
            tmp += s[i];
            ++i;
        }
    }
    return vec;
}

std::vector<std::tuple<int,int,int,int,std::vector<double>>> readRequestData(std::string filename) {
    std::vector<std::tuple<int, int, int, int, std::vector<double>>> res;
    std::ifstream infstream;
    infstream.open(filename, std::ios::in);
    int PID, SID, RID, criteria;
    std::string infoStr;
    while (!infstream.eof()) {
        infstream >> PID >> SID >> RID >> criteria >> infoStr;
        // std::cout << infoStr << std::endl;
        auto info = split(infoStr);
        res.emplace_back(std::tuple<int, int, int, int, std::vector<double>>{PID, SID, RID, criteria, info});
    }
    infstream.close();
    return res;
}

void isRepoExist(std::string prefix) {
    std::fstream file;
    file.open(prefix, std::ios::in);
    if (!file) {
        std::cout << "mkdir " << prefix << std::endl;
        std::string cmd = "mkdir -p " + prefix;
        int ret = system(cmd.c_str());
        if (ret != 0)
            exit(-1);
    }
}

void writePath(std::string filename, path p) {
    std::ofstream outfstream;
    outfstream.open(filename, std::ios::app);
    auto pAttr = p.getPathAttr();
    outfstream << pAttr.getBand() << " " << pAttr.getDelay() << " " << pAttr.getCom() << std::endl;
    outfstream.close();
}

template<typename T>
void writeEntryNum(std::string filename, std::string prefix, T cnt1, T cnt2) {
    std::ofstream outfstream;
    outfstream.open(filename, std::ios::app);
    outfstream << prefix << "\t" << cnt1 << "\t" << cnt2 << std::endl;
    outfstream.close();
}

#endif