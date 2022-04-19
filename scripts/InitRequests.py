import numpy

def InitRequest(n, propK, requestNum, SIDNum, AS):
    # n = 50
    # propIngress = 0.5
    # propK = 0.9
    # ingressNum = int(n*propIngress)
    # requestNum = 100
    # SIDNum = 12
    # [PID,SID,pos,Criteria,[cominfo]]
    pos = list(numpy.random.randint(low=0, high=n, size=requestNum))
    SID = list(numpy.random.randint(low=1, high=SIDNum+1, size=requestNum))
    PID = list(numpy.arange(0, requestNum))
    Criteria = list(numpy.random.randint(low=1, high=7, size=requestNum))
    info = []
    for c in Criteria:
        if c == 0 or c == 1 or c == 2:
            info.append([])
        elif c == 3:
            K = abs(numpy.random.normal(20, 5, 1)[0])
            info.append([K])
        elif c == 4:
            W = abs(numpy.random.normal(15, 5, 1)[0])
            info.append([W])
        elif c == 5:
            K = abs(numpy.random.normal(20, 5, 1)[0])
            _K = propK * K
            info.append([K, _K])
        elif c == 6:
            K = abs(numpy.random.normal(20, 5, 1)[0])
            _K = propK * K
            W = abs(numpy.random.normal(15, 5, 1)[0])
            info.append([K, _K, W])


    fileRequestName = "../dataSets/%s/Requests.data" % AS
    fileRequest = open(fileRequestName, "w+")
    for i in range(0, requestNum):
        infoStr = str(info[i]).replace(' ', '')
        fileRequest.write("%d\t%d\t%d\t%d\t%s\n" % (PID[i], SID[i], pos[i], Criteria[i], infoStr))
