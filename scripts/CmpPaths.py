import numpy
import os

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

def notWorse(info, p1, p2, cri):
    return not better(info, p2, p1, cri)

def equalPath(info, p1, p2, cri):
    return not(better(info, p1, p2, cri) or better(info, p2, p1, cri))

cnt1 = 0
cnt2 = 0
for AS in ["AS1221", "AS1239", "AS1755", "AS3257", "AS3967", "AS6461"]:
    # AS = "AS1221"
    fileNamePath = []
    fileNameRequest = "../dataSets/" + AS + "/Requests.data"
    fileNameCmp = "../output/" + AS + "/Cmp.data"
    for i in range(0, 3):
        fileNamePath.append("../output/" + AS + "/" + str(i) + "/paths.tsv")
    with open(fileNameCmp, "w+") as fileCmp, open(fileNameRequest) as fileRequest, open(fileNamePath[0]) as filePath0, open(fileNamePath[1]) as filePath1, open(fileNamePath[2]) as filePath2:
        for request, path0Str, path1Str, path2Str in zip(fileRequest, filePath0, filePath1, filePath2):
            reqArg = request.split("\t")
            cri = int(reqArg[3])
            # print(cri)
            infoStr = reqArg[4].strip("[]\n").split(",")
            info = []
            if cri > 2:
                info = list(map(float, infoStr))
                # print(info)
            path0 = list(map(int, path0Str.strip().split(" ")))
            path1 = list(map(int, path1Str.strip().split(" ")))
            path2 = list(map(int, path2Str.strip().split(" ")))

            # cmp = []
            # if better(info, path0, path1, cri):
            #     cmp.append("0 > 1")
            # elif better(info, path1, path0, cri):
            #     cmp.append("0 < 1")
            # else:
            #     cmp.append("0 = 1")
            # if better(info, path0, path2, cri):
            #     cmp.append("0 > 2")
            # elif better(info, path2, path0, cri):
            #     cmp.append("0 < 2")
            # else:
            #     cmp.append("0 = 2")
            # fileCmp.write(str(cmp) + "\n")

            cmp = []
            if better(info, path0, path1, cri):
                cmp.append("better")
                cnt1 += 1
            elif better(info, path1, path0, cri):
                cmp.append("worse")
            else:
                cmp.append("equal")
            if better(info, path0, path2, cri):
                cmp.append("better")
                cnt2 += 1
            elif better(info, path2, path0, cri):
                cmp.append("worse")
            else:
                cmp.append("equal")
            # if "worse" in cmp or "better" in cmp:
            #     print(cmp)
            fileCmp.write(str(cmp) + "\n")
print(cnt1, cnt2)