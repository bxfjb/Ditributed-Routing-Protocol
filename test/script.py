import functools


def better(info, p1, p2, cri):
    if cri == 1:
        return p1[0] > p2[0] or (p1[0] == p2[0] and p1[1] < p2[1])
    elif cri == 2:
        return p1[1] < p2[1] or (p1[1] == p2[1] and p1[0] > p2[0])
    elif cri == 3:
        T1 = info[0] / p1[0] + p1[1]
        T2 = info[0] / p2[0] + p2[1]
        return T1 < T2 or (T1 == T2 and p1[0] > p2[0])
    elif cri == 4:
        if p1[0] >= info[0]:
            return p2[0] < info[0] or better(info, p1, p2, 2)
        else:
            return better(info, p1, p2, 1)
    elif cri == 5:
        T1 = info[0] / p1[0] + p1[1] + info[1] / p1[2]
        T2 = info[0] / p2[0] + p2[1] + info[1] / p2[2]
        return T1 < T2 or (T1 == T2 and p1[0] > p2[0])
    elif cri == 6:
        T1 = info[0] / p1[0] + p1[1] + info[1] / p1[2]
        T2 = info[0] / p2[0] + p2[1] + info[1] / p2[2]
        if p1[0] >= info[2]:
            return p2[0] < info[2] or better(info, p1, p2, 5)
        else:
            return p1[0] > p2[0] or (p1[0] == p2[0] and T1 < T2)
    else:
        M1 = 1.0 / p1[0] + p1[1]
        M2 = 1.0 / p2[0] + p2[1]
        return M1 < M2

# for AS in ["AS1221", "AS1239", "AS1755", "AS3257", "AS3967", "AS6461"]:
#     for t in ["0", "1", "2"]:
#         print("./main " + AS + " " + t)
info = [9.854312030222241,8.868880827200018,16.895784215003413]
p1 = [14,67,7]
p2 = [5,31,7]
p3 = [14,67,7]
cri = 6
p = [p1,p2,p3]
cmp1 = better(info, p1, p2, cri)
cmp2 = better(info, p2, p1, cri)
print(cmp1, cmp2)