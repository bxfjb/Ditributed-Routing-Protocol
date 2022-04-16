#include <iostream>
#include <unordered_map>
#include <sys/time.h>
#include <tuple>
#include <string>
#include "router.h"
#include "event.h"
#include "readData.h"
#include "macro.h"

std::unordered_map<std::string, int> map {
    {"0", PARTIAL_ORDER_PATH},
    {"1", EIGRP_METRIC_PATH},
    {"2", PARTIAL_ORDER_NOCOM_PATH}
};

void shell(router **r, int type) {
    int x = 0;
    while (x != -1) {
        r[x]->printInfo(type);
    
        std::cin >> x;
    }
}

int connection(int routerNum, std::string dataSetName, std::vector<int> sid, std::vector<int> com, std::vector<std::string> name, std::vector<std::vector<std::pair<int,int>>> links, std::vector<std::tuple<int,int,int,int,std::vector<double>>> req, int type) {
    std::string prefix = "../output/" + dataSetName + "/" + std::to_string(type);
    std::string outputPathFileName = prefix + "/paths.tsv";
    std::string outputEntryNumFileName = prefix + "/entryNum.tsv";
    isRepoExist(prefix);
    std::ofstream outfstream;
    outfstream.open(outputPathFileName, std::ios::out);
    outfstream.close();
    outfstream.open(outputEntryNumFileName, std::ios::out);
    outfstream.close();
    
    router *r[routerNum];
    std::queue<router*> execQueue;
    for (int i = 0;i < routerNum;++i) {
        if (sid[i]) {
            r[i] = new router(i, name[i], sid[i], com[i], type);
            execQueue.push(r[i]);
        }
        else
            r[i] = new router(i, name[i], type);
    }
    int tmp[2];
    for (int i = 0;i < links.size();++i)
        for (int j = i+1;j < links[i].size();++j) {
            if (links[i][j].first != -1) {
                auto metric = links[i][j];
                tmp[0] = i, tmp[1] = j;
                link l1(metric.first, metric.second, tmp);
                r[i]->connectToPeer(r[j], l1);
                tmp[0] = j, tmp[1] = i;
                link l2(metric.first, metric.second, tmp);
                r[j]->connectToPeer(r[i], l2);
            }
        }
    while (!execQueue.empty())
    {
        auto node = execQueue.front();
        //std::cout << node->getID() << std::endl;
        execQueue.pop();
        if (node->isReadyRead()) {
            //std::cout << "Read" << std::endl;
            node->selection();
            execQueue.push(node);
        }
        if (node->isReadyWrite()) {
            //std::cout << "Write" << std::endl;
            node->advertise(execQueue);
        }
        if (node->isReadyReadInvalid()) {
            //std::cout << "Read Invalid" << std::endl;
            node->selectionInvalid();
            execQueue.push(node);
        }
        if (node->isReadyWriteInvalid()) {
            //std::cout << "Write Invalid" << std::endl;
            node->advertiseInvalidation(execQueue);
        }
    }
    
    int total = 0;
    for (int i = 0;i < routerNum;++i) {
        int tmp = r[i]->entryCnt();
        total += tmp;
        writeEntryNum(outputEntryNumFileName, std::to_string(i), tmp);
    }
    writeEntryNum(outputEntryNumFileName, "Total", total);

    std::cout << "Connection completed" << std::endl;

    // shell(r, type);

    event *e[REQUEST_NUM];
    for (int i = 0;i < REQUEST_NUM;++i) {
        auto request = req[i];
        e[i] = new requestEve(r, std::get<0>(request), std::get<1>(request), std::get<2>(request), std::get<3>(request), std::get<4>(request));
        e[i]->eventExec(outputPathFileName);
        delete e[i];
        e[i] = nullptr;
    }

    for (int i = 0;i < routerNum;++i) {
        delete r[i];
        r[i] = nullptr;
    }
};

int main(int argc, char* argv[]) {
    std::string dataSetName = argv[1];
    auto links = readLinkData("../dataSets/" + dataSetName + "/WidthLength.tsv");
    auto names = readNameData("../dataSets/" + dataSetName + "/Names.data");
    auto com = readComData("../dataSets/" + dataSetName + "/Computation.data");
    auto req = readRequestData("../dataSets/" + dataSetName + "/Requests.data");
    std::string routingTypeStr = argv[2];
    if (!map.count(routingTypeStr)) {
        std::cout << "Parameter 2: " << routingTypeStr << " is NOT VALID" << std::endl;
        exit(-1);
    }
    int routingType = map[routingTypeStr];
    struct timeval t1, t2;
    gettimeofday(&t1, NULL);
    connection(links.size(), dataSetName, com.first, com.second, names, links, req, routingType);
    gettimeofday(&t2, NULL);
    long long timeSub = (t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec);
    std::cout << dataSetName << " " << routingTypeStr << std::endl << " Time Cost: " << timeSub << "us" << std::endl;
    // TODO 
    // Events: Links failure and addtional ingress node 
}