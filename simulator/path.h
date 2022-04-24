#ifndef _PATH_H_
#define _PATH_H_

#include <iostream>
#include <limits.h>
#include <algorithm>
#include "macro.h"

class attr {
    private:
        int band;
        int delay;
        int computation;
    public:
        /* Default constructor */
        attr():band(INT_MAX), delay(0), computation(0) {}
        /* Initializer for paths in ingress nodes */
        attr(int c): band(INT_MAX), delay(0), computation(c) {}
        /* Initializer for links */
        attr(int b, int d): band(b), delay(d), computation(0) {}
        /* Initializer for path extension */
        attr(attr a1, attr a2) {
            band = std::min(a1.getBand(), a2.getBand());
            delay = a1.getDelay() + a2.getDelay();
            computation = a1.getCom() + a2.getCom();
        }
        /* Universal initializer */
        attr(int b, int d, int c): band(b), delay(d), computation(c) {} 
        int getBand() const {return band;}
        int getDelay() const {return delay;}
        int getCom() const {return computation;}
        /* Overloaded of > */
        bool operator >(const attr& a) const {
            if ((band == a.band) && (delay == a.delay) && (computation == a.computation))
                return false;
            return (band >= a.band) && (delay <= a.delay) && (computation >= a.computation);
        }
};

class link {
    private:
        attr linkAttr;
        /* endpoint[0] == self*/
        /* endpoint[1] == peer */
        int endpoint[2];
    public:
        /* Default constructor */
        link(): linkAttr() {}
        /* Link Initializer */
        link(int b, int d, int id[2]): linkAttr(b,d) {
            endpoint[0] = id[0];
            endpoint[1] = id[1];
        }
        attr getLinkAttr() const {return linkAttr;}
        int getSelf() const {return endpoint[0];}
        int getPeer() const {return endpoint[1];}
};

class path{
    private:
        int SID;
        int inLabel, outLabel;
        int nextHop;
        attr pathAttr;
        int pathType;
        int hop;
    public:
        /* Default constructor */
        path(): SID(INVALID_SID), inLabel(NONE_LABEL), outLabel(NONE_LABEL), nextHop(NXTHOP_SELF), pathAttr(), pathType(PARTIAL_ORDER_PATH), hop(0) {}
        /* Initializer for ingress nodes */
        path(int s, int il, int c, int t): 
            SID(s), inLabel(il), outLabel(NONE_LABEL), nextHop(NXTHOP_SELF), pathAttr(c), pathType(t), hop(0) {}
        /* Initializer for intermidiate nodes */
        path(path p, link l, int il, int t):
            SID(p.getSID()), inLabel(il), outLabel(p.getInLabel()),
                nextHop(l.getPeer()), pathAttr(p.getPathAttr(), l.getLinkAttr()), pathType(t), hop(p.getHop()+1) {}
        /* Initializer for test */
        path(attr a): 
            SID(INVALID_SID), inLabel(NONE_LABEL), outLabel(NONE_LABEL), nextHop(NXTHOP_SELF), pathAttr(a.getBand(), a.getDelay(), a.getCom()), pathType(PARTIAL_ORDER_PATH), hop(0) {}
        /* Overloaded of > */
        bool operator >(const path& p) const {
            bool ret;
            switch (pathType)
            {
                case EIGRP_METRIC_PATH:
                    ret = 1.0 / (double)pathAttr.getBand() + (double)pathAttr.getDelay() <
                        1.0 / (double)(p.getPathAttr().getBand()) + (double)(p.getPathAttr().getDelay());
                    break;
                case PARTIAL_ORDER_NOCOM_PATH:
                    ret = (pathAttr.getBand() >= p.getPathAttr().getBand() && pathAttr.getDelay() <= p.getPathAttr().getDelay())
                        && !(pathAttr.getBand() == p.getPathAttr().getBand() && pathAttr.getDelay() == p.getPathAttr().getDelay());
                    break;
                case PARTIAL_ORDER_PATH:
                default:
                    ret = pathAttr > p.getPathAttr();
                    break;
            }
            return ret;
        }
        attr getPathAttr() const {return pathAttr;}
        int getSID() const {return SID;}
        int getInLabel() const {return inLabel;}
        int getOUtLabel() const {return outLabel;}
        int getNextHop() const {return nextHop;}
        int getHop() const {return hop;}
        void invalidation() {SID = INVALID_SID;}
        void setPathType(int t) {pathType = t;}
};

bool operator == (const attr& a1, const attr& a2) {
    return a1.getBand() == a2.getBand() && a1.getDelay() == a2.getDelay() && a1.getCom() == a2.getCom();
}

bool operator == (const path& p1, const path& p2) {
    return p1.getPathAttr() == p2.getPathAttr();
}

#endif