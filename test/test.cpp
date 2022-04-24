#include <iostream>
#include <vector>
#include <queue>
#include <list>
#include <unordered_map>
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
    attr a1(1,2,3), a2(1,2,3), a3(2,2,3), a4(2,2,3), a5(2,3,4);
    path p1(a1), p2(a2), p3(a3), p4(a4), p5(a5);
    unordered_map<int, list<path>> m;
    m.emplace(1, list<path>{p1, p2, p3, p4, p5});
    m.emplace(2, list<path>{p3, p3, p4, p1});
    vector<int> ret;
    for (auto& tmp:m) {
        auto l(tmp.second);
        l.unique(); 
        ret.emplace_back(l.size());
        cout << l.size() << endl;
        // l.clear();
    }
    cout << ret[0] << endl;
    cout << ret[1] << endl;
    cout << m[1].size() << endl;
    cout << m[2].size() << endl;
}