import numpy
import os
import matplotlib.pyplot as plt

def better(info, p1, p2, cri):
    # 0 delay improved
    # 1 bandwidth improved
    # 2 unmet bandwidth is met
    if cri == 1:
        if p1[0] > p2[0]:
            return [1, 100*(p1[0] - p2[0])/p2[0]]
        elif (p1[0] == p2[0] and p1[1] < p2[1]):
            return [0, 100*(p2[1] - p1[1])/p2[1]]
        else:
            return False
    elif cri == 2:
        if p1[1] < p2[1]:
            return [0, 100*(p2[1] - p1[1])/p2[1]]
        elif (p1[1] == p2[1] and p1[0] > p2[0]):
            return [1, 100*(p1[0] - p2[0])/p2[0]]
        else:
            return False
    elif cri == 3:
        T1 = info[0] / p1[0] + p1[1]
        T2 = info[0] / p2[0] + p2[1]
        if T1 < T2:
            return [0, (T2 - T1)/T2*100]
        elif T1 == T2 and p1[0] > p2[0]:
            return [1, 100*(p1[0] - p2[0])/p2[0]]
        else:
            return False 
    elif cri == 4:
        if p1[0] >= info[0]:
            if p2[0] < info[0]:
                return [1, 100*(p1[0] - p2[0])/p2[0]]
            else:
                return better(info, p1, p2, 2)
        else:
            return better(info, p1, p2, 1)
    elif cri == 5:
        T1 = info[0] / p1[0] + p1[1] + info[1] / p1[2]
        T2 = info[0] / p2[0] + p2[1] + info[1] / p2[2]
        if T1 < T2:
            return [0, (T2 - T1)/T2*100]
        elif T1 == T2 and p1[0] > p2[0]:
            return [1, 100*(p1[0] - p2[0])/p2[0]]
        else:
            return False
    elif cri == 6:
        T1 = info[0] / p1[0] + p1[1] + info[1] / p1[2]
        T2 = info[0] / p2[0] + p2[1] + info[1] / p2[2]
        if p1[0] >= info[2]:
            if p2[0] < info[2]:
                return [1, 100*(p1[0] - p2[0])/p2[0]]
            else:
                return better(info, p1, p2, 5)
        else:
            if p1[0] > p2[0]:
                return [1, 100*(p1[0] - p2[0])/p2[0]]
            elif p1[0] == p2[0] and T1 < T2:
                return [0, (T2 - T1)/T2*100]
            else:
                return False
    else:
        M1 = 1.0 / p1[0] + p1[1]
        M2 = 1.0 / p2[0] + p2[1]
        return M1 < M2

def notWorse(info, p1, p2, cri):
    return not better(info, p2, p1, cri)

def equalPath(info, p1, p2, cri):
    return not(better(info, p1, p2, cri) or better(info, p2, p1, cri))

def meanList(l):
    if len(l) == 0:
        return 0.0
    return sum(l)/len(l)

cnt1 = 0
cnt2 = 0

data = [[[[], []] for i in range(2)] for j in range(6)]


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
            infoStr = reqArg[4].strip("[]\n").split(",")
            info = []
            if cri > 2:
                info = list(map(float, infoStr))
            path0 = list(map(int, path0Str.strip().split(" ")))
            path1 = list(map(int, path1Str.strip().split(" ")))
            path2 = list(map(int, path2Str.strip().split(" ")))

            cmp = [cri]
            cmp01 = better(info, path0, path1, cri)
            cmp10 = better(info, path1, path0, cri)
            cmp02 = better(info, path0, path2, cri)
            cmp20 = better(info, path2, path0, cri)
            if cmp01:
                # band
                data[cri-1][0][cmp01[0]].append(cmp01[1])
                cmp.append(cmp01)
                cnt1 += 1
            elif cmp10:
                cmp.append("worse")
            else:
                cmp.append("equal")
            if cmp02:
                # delay
                data[cri-1][1][cmp02[0]].append(cmp02[1])
                cmp.append(cmp02)
                cnt2 += 1
            elif cmp20:
                cmp.append("worse")
            else:
                cmp.append("equal")
            # if "worse" in cmp or "better" in cmp:
            #     print(cmp)
            fileCmp.write(str(cmp) + "\n")
            # print(cmp)

x_label = ["Shortest-\nWidest", "Widest-\nShortest", "K-Quickest", "W-Wide-\nShortest", "Neo-K-\nQucikest", "Neo-W-Wide-\nShortest"]
proto_label = ["EIGRP", "PDP"]
band_delay_label = ["Bandwidth Improved", "Delay Improved"]
width = 0.3
x = numpy.arange(len(x_label))

num1 = []
num2 = []
num_labels = []
percent1_band = []
percent1_delay = []
percent1_labels = []
percent2_band = []
percent2_delay = []
percent2_labels = []

cnt = [0, 0]
for i in range (6):
    num1.append(len(data[i][0][0]) + len(data[i][0][1]))
    num2.append(len(data[i][1][0]) + len(data[i][1][1]))
    percent1_band.append(meanList(data[i][0][0]))
    percent1_delay.append(meanList(data[i][0][1]))
    percent2_band.append(meanList(data[i][1][0]))
    percent2_delay.append(meanList(data[i][1][1]))

num_labels.append(proto_label[0] + "(%.1f)" % (sum(num1)/6))
num_labels.append(proto_label[1] + "(%.1f)" % (sum(num2)/6))
percent1_labels.append(band_delay_label[0] + "(%.2f%%)" % (sum(percent1_band)/6))
percent1_labels.append(band_delay_label[1] + "(%.2f%%)" % (sum(percent1_delay)/6))
percent2_labels.append(band_delay_label[0] + "(%.2f%%)" % (sum(percent2_band)/6))
percent2_labels.append(band_delay_label[1] + "(%.2f%%)" % (sum(percent2_delay)/6))
    
plt.xlabel("Optimal Criteria")
plt.ylabel("Number of Improved Requests")
plt.bar(x, num1, width, align="center", color="blue", label=num_labels[0], alpha=0.3)
plt.bar(x+width, num2, width, align="center", color="red", label=num_labels[1], alpha=0.3) # PDP is Protocol for Dominant Paths
plt.xticks(x+width/2, x_label, fontsize=10)
plt.legend()
plt.savefig("../plots/Number_of_Improved_Requests.pdf")
# plt.show()

plt.clf()

plt.xlabel("Optimal Criteria")
plt.ylabel("Performance Improvement Compare to EIGRP")
plt.bar(x, percent1_band, width, align="center", color="green", label=percent1_labels[0], alpha=0.3)
plt.bar(x+width, percent1_delay, width, align="center", color="orange", label=percent1_labels[1], alpha=0.3)
plt.xticks(x+width/2, x_label)
plt.ylim(0, 120)
plt.legend()
# plt.show()
plt.savefig("../plots/Performance_Improvement_EIGRP.pdf")

plt.clf()

plt.xlabel("Optimal Criteria")
plt.ylabel("Performance Improvement Compare to PDP")
plt.bar(x, percent2_band, width, align="center", color="green", label=percent2_labels[0], alpha=0.3)
plt.bar(x+width, percent2_delay, width, align="center", color="orange", label=percent2_labels[1], alpha=0.3)
plt.xticks(x+width/2, x_label)
plt.ylim(0, 120)
plt.legend()
# plt.show()
plt.savefig("../plots/Performance_Improvement_PDP.pdf")

print(cnt1, cnt2)