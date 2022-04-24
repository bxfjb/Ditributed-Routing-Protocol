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
distinctEntryNum = [[0, 0] for i in range(3)]
entryNum = [[0, 0] for i in range(3)]

totalEntryNum = [0, 0, 0]
totalDistinctEntryNum = [0, 0, 0]
AvgRouterEntryNum = [0, 0, 0]
AvgRouterDistinctEntryNum = [0, 0, 0]
AvgSidEntryNum = [0, 0, 0]
AvgSidDistinctEntryNum = [0, 0, 0]

for AS in ["AS1221", "AS1239", "AS1755", "AS3257", "AS3967", "AS6461"]:
    # AS = "AS1221"
    fileNamePath = []
    fileNameRequest = "../dataSets/" + AS + "/Requests.data"
    fileNameCmp = "../output/" + AS + "/Cmp.data"
    for i in range(0, 3):
        fileNamePath.append("../output/" + AS + "/" + str(i) + "/paths.tsv")
    with open(fileNameCmp, "w+") as fileCmp, open(fileNameRequest) as fileRequest, \
         open(fileNamePath[0]) as filePath0, open(fileNamePath[1]) as filePath1, open(fileNamePath[2]) as filePath2:
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

    fileNameEntryNum = []
    for i in range(0, 3):
        fileNameEntryNum.append("../output/" + AS + "/" + str(i) + "/entryNum.tsv")

    with open(fileNameEntryNum[0]) as fileEntryNum0,\
        open(fileNameEntryNum[1]) as fileEntryNum1, open(fileNameEntryNum[2]) as fileEntryNum2:
        i = 0
        for arg0, arg1, arg2 in fileEntryNum0, fileEntryNum1, fileEntryNum2:
            arg0 = arg0.strip("\n").split("\t")
            arg1 = arg1.strip("\n").split("\t")
            arg2 = arg2.strip("\n").split("\t")
            totalEntryNum[i] += float(arg0[1])
            totalDistinctEntryNum[i] += float(arg0[2])
            AvgRouterEntryNum[i] += float(arg1[1])
            AvgRouterDistinctEntryNum[i] += float(arg1[2])
            AvgSidEntryNum[i] += float(arg2[1])
            AvgSidDistinctEntryNum[i] += float(arg2[2])
            i += 1

x_label = ["CPDP", "EIGRP", "PDP"]
data_label =  ["Distinct entry number", "Entry number"]
x = numpy.arange(len(x_label))
width = 0.3

plt.xlabel("Protocols")
plt.ylabel("Number of entry")
plt.bar(x,[num/6 for num in AvgRouterDistinctEntryNum], width, align="center", color="green", label=data_label[0], alpha=0.3)
plt.bar(x+width, [num/6 for num in AvgRouterEntryNum], width, align="center", color="orange", label=data_label[1], alpha=0.3)
plt.xticks(x+width/2, x_label)
# plt.ylim(0, 120)
plt.legend()
# plt.show()
plt.savefig("../plots/Number_of_Entry_Per_Router.pdf")

plt.clf()

plt.xlabel("Protocols")
plt.ylabel("Number of entry")
plt.bar(x, [num/6 for num in AvgSidDistinctEntryNum], width, align="center", color="green", label=data_label[0], alpha=0.3)
plt.bar(x+width, [num/6 for num in AvgSidEntryNum], width, align="center", color="orange", label=data_label[1], alpha=0.3)
plt.xticks(x+width/2, x_label)
# plt.ylim(0, 120)
plt.legend()
plt.savefig("../plots/Number_of_Entry_Per_SID.pdf")

plt.clf()

x_label = ["Shortest-\nWidest", "Widest-\nShortest", "K-Quickest", "W-Wide-\nShortest", "Neo-K-\nQucikest", "Neo-W-Wide-\nShortest"]
proto_label = ["EIGRP", "PDP"]
band_delay_label = ["Bandwidth Improved", "Delay Improved"]
width = 0.3
x = numpy.arange(len(x_label))
proto_x = numpy.arange(len(proto_label))

num1 = []
num2 = []
num_labels = []
band = [0, 0]
delay = [0, 0]
band_labels = []
delay_labels = []

cnt = [0, 0]
for i in range (6):
    num1.append(len(data[i][0][0]) + len(data[i][0][1]))
    num2.append(len(data[i][1][0]) + len(data[i][1][1]))
    band[0] += meanList(data[i][0][0])
    delay[0] += meanList(data[i][0][1])
    band[1] += meanList(data[i][1][0])
    delay[1] += meanList(data[i][1][1])

num_labels.append(proto_label[0] + "(%d)" % sum(num1))
num_labels.append(proto_label[1] + "(%d)" % sum(num2))

    
plt.xlabel("Optimal Criteria")
plt.ylabel("Number of Improved Requests")
plt.bar(x, num1, width, align="center", color="blue", label=num_labels[0], alpha=0.3)
plt.bar(x+width, num2, width, align="center", color="red", label=num_labels[1], alpha=0.3) # PDP is Protocol for Dominant Paths
plt.xticks(x+width/2, x_label, fontsize=10)
plt.legend()
plt.savefig("../plots/Number_of_Improved_Requests.pdf")
# plt.show()

plt.clf()

x = numpy.arange(len(proto_label))
fig, ax1 = plt.subplots()

ax1.set_ylabel('Bandwidth improvement')
ax1.set_ylim(0, 30)
ax1.bar(x, band, width=width, color='lightseagreen', align='edge', label=band_delay_label[0])
ax1.legend()

# ax1.set_xticklabels(ax1.get_xticklabels()) 

ax2 = ax1.twinx()
ax2.set_ylabel('Delay improvement')
ax2.set_ylim(0, 400)
ax2.bar(x+width, delay, width=width, color='tab:blue', align='edge', tick_label=proto_label, label=band_delay_label[1])
ax2.legend(loc=2)

plt.tight_layout()
plt.savefig("../plots/Performance_Improvement.pdf")

plt.clf()

print(cnt1, cnt2)