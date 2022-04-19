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

data = [[[[], []] for i in range(2)] for j in range(6)]
data[0][0][1].append(0)
print(data)