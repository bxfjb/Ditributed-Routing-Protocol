import os
import time

fileExecName = "../simulator/main"
fileNetwokDatasetName = "./NetworkDataSets.py"
filePlotName = "./Plot.py"

start = time.time()

os.system("%s %s" % ("python3", fileNetwokDatasetName))
for AS in ["AS1221", "AS1239", "AS1755", "AS3257", "AS3967", "AS6461"]:
    for t in ["0", "1", "2"]:
        os.system("%s %s %s" % (fileExecName, AS, t))
os.system("%s %s" % ("python3", filePlotName))

end = time.time()

print("Time cost: %s second" % str(end-start))