#ifndef _EVENT_H_
#define _EVENT_H_

#include <sys/time.h>
#include <iostream>
#include "macro.h"
#include "router.h"

class event{
    protected:
        router **routerArray;
        struct timeval beginTime, endTime;
        long long timeSub;
    public:
        event(router **r) {
            routerArray = r;
            gettimeofday(&beginTime, NULL);
        }
        virtual ~event() {
            gettimeofday(&endTime, NULL);
            timeSub = (endTime.tv_sec - beginTime.tv_sec) * 1000000 + (endTime.tv_usec - beginTime.tv_usec);
            // std::cout << "Event time cost: " << timeSub << "us" << std::endl;
        }
        virtual void eventExec(std::string outFileName) = 0;
};
/* Intermidiate nodes: Lable match RIB 
 * Ingress nodes: Check packageType
 *      0' NORMAL: PID match FIB, then forward
 *      1' HEAD: fill FIB according to RIB, then dump
 *      2' TAIL: delete PID entry from FIB, then dump
 */
class package {
    protected:
        int packageType;
        int PID;            // simulate 5-tuple
        int label;
        int pos;
    public:
        package(): packageType(NORMAL_PACKAGE), PID(-1), label(NONE_LABEL), pos(INVALID_RID) {}
        /* Constructor*/
        package(int t): packageType(t), PID(-1), label(NONE_LABEL), pos(INVALID_RID) {}
        package(int t, int pi, int p): packageType(t), PID(pi), label(NONE_LABEL), pos(p) {}
        virtual void execPackage(router **r, std::string outFileName) = 0;
};

/* The very first package */
class headPackage: public package {
    private:
        int SID;
        int Criteria;
        // /* For crit 3 5 6 */
        // double K;
        // /* For crit 5 6 */
        // double _K;
        // /* For crit 4 6 */
        // double W;
        std::vector<double> info;
    public:
        headPackage(): package(HEAD_PACKAGE), SID(INVALID_SID), Criteria(EIGRP_METRIC) {}
        /* Constructor for 0 1 2 */
        headPackage(int s, int c, int pi, int p): package(HEAD_PACKAGE, pi, p), SID(s), Criteria(c) {}
        /* Constructor for 3 4 5 6 */
        headPackage(int s, int c, int pi, int p, std::vector<double> &com): package(HEAD_PACKAGE, pi, p), SID(s), Criteria(c), info(com) {}
        /* Parse head packet and fill FIB, then dump it */
        void execPackage(router **r, std::string outFileName) {
            r[pos]->execHeadPackage(PID, SID, Criteria, info, outFileName);
            // std::cout << "Package[" << PID << "]: ";
        }
};

class normalPackage: public package {
    public:
        normalPackage(): package(NORMAL_PACKAGE) {}
        normalPackage(int pi, int p): package(NORMAL_PACKAGE, pi, p) {}
        void execPackage(router **r, std::string outFileName) {
            while (1) {
                auto action = r[pos]->execNormalPackage(PID, label);
                if (action.first != NXTHOP_SELF) {
                    // std::cout << "[" << pos << "](" << label << ")-->";
                    pos = action.first;
                    label = action.second;
                }
                else {
                    // std::cout << "[" << pos << "]" << std::endl;
                    break;
                }
            }
        }
};

class tailPackage: public package {
    public:
        tailPackage(): package(TAIL_PACKAGE) {}
        tailPackage(int pi, int p): package(TAIL_PACKAGE, pi, p) {}
        void execPackage(router **r, std::string outFileName) {
            r[pos]->execTailPackage(PID);
        }
};

class requestEve: public event {
    private:
        int ingressID;
        package *p[3];
    public:
        requestEve(router **r): event(r), ingressID(INVALID_RID) {
            p[0] = nullptr;
            p[1] = nullptr;
            p[2] = nullptr;
        }
        requestEve(router **r, int PID, int SID, int pos, int Criteria, std::vector<double> &info): event(r), ingressID(pos) {
            if (Criteria >= 0 && Criteria <= 2)
                p[0] = new headPackage(SID, Criteria, PID, ingressID);
            else
                p[0] = new headPackage(SID, Criteria, PID, ingressID, info);
            p[1] = new normalPackage(PID, ingressID);
            p[2] = new tailPackage(PID, ingressID);
        }
        ~requestEve() {
            delete p[0];
            p[0] = nullptr;
            delete p[1];
            p[1] = nullptr;
            delete p[2];
            p[2] = nullptr;
        }
        void eventExec(std::string outFileName) {
            p[0]->execPackage(routerArray, outFileName);
            p[1]->execPackage(routerArray, outFileName);
            p[2]->execPackage(routerArray, outFileName);
        }
};

class announcementEve : public event {

};

class linkFaliureEve : public event {
    
};

#endif