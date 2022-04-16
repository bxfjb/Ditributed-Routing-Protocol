#ifndef _ROUTER_H_
#define _ROUTER_H_

#include "path.h"
#include "macro.h"
#include "readData.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <string>
#include <queue>
#include <list>
#include <set>
#include <iomanip>

class router {
    private:
        /* Unique Integer identification */
        int rouetrId;
        std::string name;
        /* RIB */
        std::unordered_map<int,std::list<path>> RIB; // use list for path delete
        std::set<int> SIDList;
        std::vector<router*> peers; // use vector for paths advertisement
        std::unordered_map<int,link> links;
        /* For label match */
        std::unordered_map<int,path> inLableMap; 
        /* FIB */
        std::unordered_map<int,path> FIB;
        /* Computation capability related */
        int SID;
        int comCapa;
        bool isIngress;
        /* label count */
        int inLabelCnt = 0, outLabelCnt = 0;
        /* advertise related */
        std::list<path> adjOut;
        std::list<std::pair<path,int>> adjIn;
        std::list<path> adjOutInvalid;
        std::list<std::pair<path,int>> adjInInvalid;
        /* routing type */
        int type;
    public:
        /* Default constructor */
        router(): rouetrId(-1), name("NAR"), SID(INVALID_SID), comCapa(0), isIngress(false), type(PARTIAL_ORDER_PATH) {}
        /* Initializer for ingress nodes */
        router(int id, std::string n, int sid, int com, int t): 
            rouetrId(id), name(n), SID(sid), comCapa(com), isIngress(true), type(t) {
            path p(SID, ++inLabelCnt, comCapa, type);
            RIB.emplace(SID, std::list<path>{p});
            inLableMap.emplace(p.getInLabel(), p);
            adjOut.emplace_back(p);
            SIDList.emplace(sid);
        }
        /* Initializer for intermidiate nodes */
        router(int id, std::string n, int t): 
            rouetrId(id), name(n), SID(INVALID_SID), comCapa(0), isIngress(false), type(t) {}
        /* Getters and setters */
        int getID() const {return rouetrId;}
        std::string getName() const {return name;}
        int getSID() const {return SID;}
        bool isReadyRead() {return !adjIn.empty();}
        bool isReadyWrite() {return !adjOut.empty();}
        bool isReadyReadInvalid() {return !adjInInvalid.empty();}
        bool isReadyWriteInvalid() {return !adjOutInvalid.empty();}

        /* We need to call this function twice to connect 2 rouetrs. */
        /* Para l is different between the 2 calling. */
        void connectToPeer(router* p, link l) {
            int peerId = p->getID();
            if (!links.count(peerId)) {
                peers.emplace_back(p);
                links.emplace(peerId, l);
            }
        }
        
        int entryCnt() {
            int ans = 0;
            for (auto pathSet:RIB)
                ans += pathSet.second.size();
            return ans;
        }

        /* Called by advertise() */
        void accept(path p, int peerId) {
            adjIn.emplace_back(std::pair<path,int>{p,peerId});
        }   
        /* Called by main() when readyWrite is true */
        void advertise(std::queue<router*> &q) {
            while (!adjOut.empty()) {
                auto _path = adjOut.front();
                for (auto &peer:peers) {
                    if (_path.getNextHop() != peer->getID()) {
                        peer->accept(_path, rouetrId);
                        q.push(peer);
                    }
                }
                adjOut.pop_front();
            }
        }

        void acceptInvalidation(path p, int peerId) {
            adjInInvalid.emplace_back(std::pair<path, int>{p, peerId});
        }
        void advertiseInvalidation(std::queue<router*> &q) {
            while (!adjOutInvalid.empty()) {
                auto _path = adjOutInvalid.front();
                for (auto &peer:peers) {
                    if (_path.getNextHop() != peer->getID()) {
                        peer->acceptInvalidation(_path, rouetrId);
                        q.push(peer);
                    }
                }
                adjOutInvalid.pop_front();
            }
        }

        void pathSelection(path p, int peerId) {
            link l = links[peerId];
            path _p(p, l, ++inLabelCnt, type);
            int pSID = _p.getSID();
            /* The first pSID path */
            if (!RIB.count(pSID)) {
                RIB.emplace(pSID, std::list<path>{_p});
                inLableMap.emplace(_p.getInLabel(), _p);
                SIDList.emplace(pSID);
                adjOut.emplace_back(_p);
                return;
            }
            else {
                auto pathSet = &(RIB.find(pSID)->second);
                for (auto &oldPath:*pathSet) {
                    if (oldPath > _p){
                        --inLabelCnt;
                        return;
                    }
                    if (_p > oldPath) {
                        // oldPath.setPathType(4);
                        adjOutInvalid.emplace_back(oldPath);
                        oldPath.invalidation();
                        inLableMap.erase(oldPath.getInLabel());
                    }
                }
                pathSet->remove_if([](path op){return op.getSID() == INVALID_SID;});
                pathSet->emplace_back(_p);
                inLableMap.emplace(_p.getInLabel(), _p);
                adjOut.emplace_back(_p);
            }
        }

        void pathSelectionInvalid(path p, int peerId) {
            auto pSID = p.getSID();
            auto pInLabel = p.getInLabel();
            // std::cout << "pSID: " << pSID << std::endl;
            if (!RIB.count(pSID)) {
                return;
            }
            auto pathSet = &(RIB[pSID]);
            for (auto &_path:*pathSet) {
                // std::cout << "OutLabel: " << _path.getOUtLabel();
                // std::cout << ", NextHop: " << _path.getNextHop() << std::endl;
                if (_path.getOUtLabel() == pInLabel && _path.getNextHop() == peerId) {
                    // std::cout << "Find one invalid path" << std::endl;
                    adjOutInvalid.emplace_back(_path);
                    _path.invalidation();
                    inLableMap.erase(_path.getInLabel());
                    break;
                }
            }
            pathSet->remove_if([](path op){return op.getSID() == INVALID_SID;});
        }

        /* Called by main() when readyRead is true */
        /* Deal with paths in adjIn */
        void selection() {
            while (!adjIn.empty()) {
                auto tmp = adjIn.front();
                pathSelection(tmp.first, tmp.second);
                adjIn.pop_front();
            }
        }

        void selectionInvalid() {
            while (!adjInInvalid.empty()) {
                auto tmp = adjInInvalid.front();
                pathSelectionInvalid(tmp.first, tmp.second);
                adjInInvalid.pop_front();
            }
        }

        /* For event call */
        void execHeadPackage(int PID, int SID, int Criteria, std::vector<double> &info, std::string outFileName) {
            if (!RIB.count(SID)) {
                std::cout << "Invalid SID" << std::endl;
                exit(-1);
            }
            auto pathSet = RIB[SID];
            path *chosen = nullptr;
            for (auto &p:pathSet) {
                if (chosen == nullptr)
                    chosen = &p;
                else
                    chosen = cmpPath(Criteria, &p, chosen, info) ? &p : chosen;
            }
            writePath(outFileName, *chosen);
            FIB.emplace(PID, *chosen);
        }

        std::pair<int,int> execNormalPackage(int PID, int label) {
            path *p;
            if (inLableMap.count(label)) {
                p = &inLableMap[label];
            }
            else if (FIB.count(PID)) {
                p = &FIB[PID];
            }
            else {
                std::cout << "Invalid PID and label" << std::endl;
                exit(-1);
            }
            return std::pair<int,int>{p->getNextHop(), p->getOUtLabel()};
        }

        void execTailPackage(int PID) {
            if (!FIB.count(PID)) {
                std::cout << "Invalid PID" << std::endl;
                exit(-1);
            }
            FIB.erase(PID);
        }

        /* Return true if p1 better than p2 under Criteria */
        bool cmpPath(int Criteria, path *p1, path *p2, std::vector<double> &info) {
            attr a1 = p1->getPathAttr(), a2 = p2->getPathAttr();
            if (Criteria == SHORTEST_WIDEST) {
                return a1.getBand() > a2.getBand() || 
                (a1.getBand() == a2.getBand() && a1.getDelay() < a2.getDelay());
            }
            else if (Criteria == WIDEST_SHOREST) {
                return a1.getDelay() < a2.getDelay() || 
                (a1.getDelay() == a2.getDelay() && a1.getBand() > a2.getBand());
            }
            else if (Criteria == K_QUICKEST) {
                double T1 = info[0]/a1.getBand() + a1.getDelay();
                double T2 = info[0]/a2.getBand() + a2.getDelay();
                return T1 < T2 ||(T1 == T2 && a1.getBand() > a2.getBand());
            }
            else if (Criteria == W_WIDE_SHORTEST) {
                if (a1.getBand() >= info[0])
                    return a2.getBand() < info[0] || cmpPath(WIDEST_SHOREST, p1, p2, info);
                else
                    return cmpPath(SHORTEST_WIDEST, p1, p2, info);
            }
            else if (Criteria == NEO_K_QUICKEST) {
                double T1 = info[0]/a1.getBand() + info[1]/a1.getCom() + a1.getDelay();
                double T2 = info[0]/a2.getBand() + info[1]/a2.getCom() + a2.getDelay();
                return T1 < T2 || (T1 == T2 && a1.getBand() > a2.getBand());
            }
            else if (Criteria == NEO_W_WIDE_SHORTEST) {
                double T1 = info[0]/a1.getBand() + info[1]/a1.getCom() + a1.getDelay();
                double T2 = info[0]/a2.getBand() + info[1]/a2.getCom() + a2.getDelay();
                if (a1.getBand() >= info[2])
                    return a2.getBand() < info[2] || cmpPath(NEO_K_QUICKEST, p1, p2, info);
                else
                    return a1.getBand() > a2.getBand() || (a1.getBand() == a2.getBand() && T1 < T2);
            }
            else {
                double M1 = (double)1.0 / a1.getBand() + a1.getDelay();
                double M2 = (double)1.0 / a2.getBand() + a2.getDelay();
                return M1 < M2;
            }
        }

        void printInfo(int type) {
            std::cout << "This is router[" << rouetrId << "]:" << name << std::endl;
            std::cout << "Stored label: " << std::endl;
            for (auto l:inLableMap)
                std::cout << l.first << " ";
            std::cout << std::endl;
            for (auto peer:peers) {
                std::cout << "Connectting to router[" << peer->getID() << "]:" << peer->getName() 
                    << " via link:(" << links[peer->getID()].getLinkAttr().getBand() << "," 
                                     << links[peer->getID()].getLinkAttr().getDelay() << ")" << std::endl;
            }
            if (isIngress) {
                std::cout << "*****************************************************" << std::endl;
                std::cout << "This is an ingress node with SID[" << SID << "]," 
                          << "Computational Capality:" << comCapa << std::endl;
                std::cout << "*****************************************************" << std::endl;
            }
            std::cout << "Stroed RIB: " << std::endl;
            if (type == PARTIAL_ORDER_PATH || type == PARTIAL_ORDER_NOCOM_PATH) {
                std::cout << "------------------------------------------------------------------------" << std::endl;
                std::cout << "| SID | BandWidth | Delay | Capability | inLabel | outLabel | Next Hop |" << std::endl;
                std::cout << "------------------------------------------------------------------------" << std::endl;
                for (auto _sid:SIDList){
                    for (auto _path:RIB[_sid]) {
                        std::cout << "|" << std::setw(3) << _sid << std::setw(3) << "|";
                        (_path.getPathAttr().getBand() == INT_MAX) ?
                        std::cout << std::setw(6) <<  "Inf" << std::setw(6) << "|" :
                        std::cout << std::setw(6) <<  _path.getPathAttr().getBand() << std::setw(6) << "|";
                        std::cout << std::setw(4) << _path.getPathAttr().getDelay() << std::setw(4) << "|";
                        std::cout << std::setw(7) << _path.getPathAttr().getCom() << std::setw(6) << "|";
                        std::cout << std::setw(5) << _path.getInLabel() << std::setw(5) << "|";
                        (_path.getOUtLabel() != NONE_LABEL) ? 
                        std::cout << std::setw(6) << _path.getOUtLabel() << std::setw(5) << "|" :
                        std::cout << std::setw(6) << "-" << std::setw(5) << "|";
                        (_path.getNextHop() != NXTHOP_SELF) ?
                        std::cout << std::setw(6) << _path.getNextHop() << std::setw(5) << "|":
                        std::cout << std::setw(6) << "self" << std::setw(5) << "|";
                        std::cout << std::endl;
                std::cout << "------------------------------------------------------------------------" << std::endl;
                    }
                }
            }
            else if (type == EIGRP_METRIC_PATH) {
                std::cout << "---------------------------------------------------------------------------------" << std::endl;
                std::cout << "| SID | BandWidth | Delay | Capability | inLabel | outLabel | Next Hop | Metric |" << std::endl;
                std::cout << "---------------------------------------------------------------------------------" << std::endl;
                for (auto _sid:SIDList){
                    for (auto _path:RIB[_sid]) {
                        std::cout << "|" << std::setw(3) << _sid << std::setw(3) << "|";
                        (_path.getPathAttr().getBand() == INT_MAX) ?
                        std::cout << std::setw(6) <<  "Inf" << std::setw(6) << "|" :
                        std::cout << std::setw(6) <<  _path.getPathAttr().getBand() << std::setw(6) << "|";
                        std::cout << std::setw(4) << _path.getPathAttr().getDelay() << std::setw(4) << "|";
                        std::cout << std::setw(7) << _path.getPathAttr().getCom() << std::setw(6) << "|";
                        std::cout << std::setw(5) << _path.getInLabel() << std::setw(5) << "|";
                        (_path.getOUtLabel() != NONE_LABEL) ? 
                        std::cout << std::setw(6) << _path.getOUtLabel() << std::setw(5) << "|" :
                        std::cout << std::setw(6) << "-" << std::setw(5) << "|";
                        (_path.getNextHop() != NXTHOP_SELF) ?
                        std::cout << std::setw(6) << _path.getNextHop() << std::setw(5) << "|" << std::setw(5) << (double)1.0 / _path.getPathAttr().getBand() + _path.getPathAttr().getDelay() << std::setw(4) << "|":
                        std::cout << std::setw(6) << "self" << std::setw(5) << "|" << std::setw(4) << "0" << std::setw(5) << "|";
                        std::cout << std::endl;
                std::cout << "---------------------------------------------------------------------------------" << std::endl;
                    }
                }
            }
            else
                std::cout << "NOT VALID ROUTING TYPE";
            std::cout << std::endl;

        }
};

#endif