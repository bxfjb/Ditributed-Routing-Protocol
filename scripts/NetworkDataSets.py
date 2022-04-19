# File        : Scripts/NetworkDataSets.py

# Description : A script for transforming Rocketfuel topologies into the
#               network data sets that serve as input to the simulator.

# Public Library
import networkx as nx
import os
import sys
import re
import numpy
import InitRequests

# Change to:
for AS in ['AS1221', 'AS1239', 'AS1755', 'AS3257', 'AS3967', 'AS6461']:
# to generate the test networks pertaining the all the ASs.

    # AS = "AS1221"

    fileNameWeights = '../RocketfuelTologies/%s/weights.intra' % AS
    fileNameLatencies = '../RocketfuelTologies/%s/latencies.intra' % AS

    graph = nx.Graph()

    with open(fileNameWeights) as fileWeight, open(fileNameLatencies) as fileLatencies:
        pattern = '[0-9]'

        for lineWeight, lineLatencies in zip(fileWeight, fileLatencies):
            argsWeight = lineWeight.rstrip().split(" ")
            argsLatencies = lineLatencies.rstrip().split(" ")

            node1 = argsWeight[0]
            node2 = argsWeight[1]

            width = int(100 * (1 / float(argsWeight[2])))
            length = int(float(argsLatencies[2]))

            graph.add_edge(node1, node2, Width=width, Length=length)

    print(len(list(nx.biconnected_components(graph))))
    graph = max((graph.subgraph(c) for c in nx.biconnected_components(graph)), key=len)
    nameList = []
    for label in enumerate(graph.nodes()):
        nameList.append(label[1])
    graph = nx.convert_node_labels_to_integers(graph)
    graph = graph.to_directed()

    # pos = nx.spring_layout(graph)
    # nx.draw(graph, pos)
    # nx.draw_networkx_labels(graph, pos)
    # plt.show()

    print("\n--- Test Network pertaining to %s" % AS)
    print("\tNumber of Nodes: %d" % graph.number_of_nodes())
    print("\tNumber of Links: %d" % graph.number_of_edges())
    print("\tNumber of Distinct Widths: %d" % len(set([width for _, _, width in graph.edges.data('Width')])))

    for Attribute in [['Width', 'Length']]:

        dirNameTestNetwork = "../dataSets/%s" % AS
        fileNameTestNetwork = "%s/%s.tsv" % (dirNameTestNetwork, ''.join(Attribute))

        try:
            os.makedirs(dirNameTestNetwork)
        except OSError as exception:
            pass

        fileTestNetwork = open(fileNameTestNetwork, "w+")

        print("\n\t+ Saving Data Set to File '%s' " % fileNameTestNetwork)

        string2file = '%s\n' % str(graph.number_of_nodes())
        fileTestNetwork.write(string2file)

        for u, v, data in graph.edges(data=True):

            string2file = '%s\t%s' % (str(u), str(v))

            if 'Width' in Attribute:
                string2file += '\t%s' % str(data['Width'])

            if 'Hops' in Attribute:
                string2file += '\t%s' % '1'

            if 'Length' in Attribute:
                string2file += '\t%s' % str(data['Length'])

            string2file += '\n'

            fileTestNetwork.write(string2file)

        fileTestNetwork.close()

    print("")

    n = graph.number_of_nodes()
    ingressProp = 0.5
    ingressNum = int(n*ingressProp)
    comIdx = numpy.random.choice(n-1, int(n*ingressProp), replace=False)
    comIdx.sort()
    comSID = numpy.ones(int(n*ingressProp), dtype=int)
    SID = 1
    for i in range(1, int(n*ingressProp)):
        if numpy.random.random_integers(2) == 1:
            SID += 1
        comSID[i] = SID
    # print(comIdx)
    # print(comSID)
    comCap = numpy.random.poisson(lam=10, size=int(n*ingressProp))
    # print(comCap)

    com = [[0] * 2 for _ in range(n)]
    for i in range(0, int(n*ingressProp)):
        com[comIdx[i]][0] = comSID[i]
        com[comIdx[i]][1] = comCap[i]
    # print(com)

    fileNameCom = "../dataSets/%s/Computation.data" % AS
    fileCom = open(fileNameCom, "w+")
    for c in com:
        fileCom.write(str(c[0]) + " " + str(c[1]) + "\n")
    fileCom.close()

    fileNameNames = "../dataSets/%s/Names.data" % AS
    fileNames = open(fileNameNames, "w+")
    for name in nameList:
        fileNames.write(str(name) + "\n")
    fileNames.close()

    InitRequests.InitRequest(n, 0.95, 10000, SID, AS)
