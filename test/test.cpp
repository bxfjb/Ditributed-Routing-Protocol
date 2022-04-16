#include <iostream>
#include <vector>
#include "../simulator/path.h"

using namespace std;

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
};

void func(vector<double> &v) {
    v.pop_back();
    v.emplace_back(3.14);
};

int main() {
    attr a1(5,31,7), a2(14,67,7);
    path p1(a1), p2(a2);
    vector<double> info{9.85, 8.87, 16.90};
    cout << cmpPath(6, &p1, &p2, info) << endl;
    cout << cmpPath(6, &p2, &p1, info) << endl;
}