import csv
import numpy
import pandas as pd
import math

date = ["01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15"]

bottomLat = 30.524081949676
topLat = 30.7938780503239
leftLon = 103.908407474531
rightLon = 104.222044525468
startTime = 1477929600
endTime = startTime + 1296000
midLat = (bottomLat + topLat) / 2
midLon = (leftLon + rightLon) / 2

data = []
label = []

maxTime = 40 * 60

count = 0
for d in date:
    for part in range(0, 5):
        filename = "order_201611" + d + "_part" + str(part) + ".csv"
        with open(filename) as f:
            reader = csv.reader(f)
            head = next(reader)
            for entry in reader:
                vec = []
                depatureTime = int(entry[1])
                arriveTime = int(entry[2])
                origLng = float(entry[3])
                origLat = float(entry[4])
                destLng = float(entry[5])
                destLat = float(entry[6])
                if depatureTime < startTime or depatureTime >= endTime:
                    continue
                if arriveTime < startTime or arriveTime >= endTime:
                    continue
                if origLat < bottomLat or origLat > topLat:
                    continue
                if origLng < leftLon or origLng > rightLon:
                    continue
                if destLat < bottomLat or destLat > topLat:
                    continue
                if destLng < leftLon or destLng > rightLon:
                    continue
                
                if((depatureTime - startTime) % 86400 > 25200 and (depatureTime - startTime) % 86400 < 32400):
                    vec.extend([1,0,0,0,0])
                elif((depatureTime - startTime) % 86400 > 61200 and (depatureTime - startTime) % 86400 < 68400):
                    vec.extend([0,1,0,0,0])
                elif((depatureTime - startTime) % 86400 > 32400 and (depatureTime - startTime) % 86400 < 61200):
                    vec.extend([0,0,1,0,0])
                elif((depatureTime - startTime) % 86400 > 68400 and (depatureTime - startTime) % 86400 < 79200):
                    vec.extend([0,0,0,1,0])
                else:
                    vec.extend([0,0,0,0,1])
                    
                label.append((arriveTime - depatureTime))
                day = int(d)
                day = (day + 1) % 7
                if day == 6 or day == 0:
                    vec.extend([0, 1])
                else:
                    vec.extend([1, 0])

                vec.append(origLng - leftLon)
                vec.append(origLat - bottomLat)
                vec.append(destLng - leftLon)
                vec.append(destLat - bottomLat)

                data.append(vec)
                count += 1
                if (count % 100000 == 0):
                    print(len(data))
                
            
df = pd.DataFrame(data)
dfl = pd.Series(label)
des = df.describe()
print(des)
print(dfl.describe())
for i in [7, 8, 9, 10]:
    if des[i]['std'] != 0:
        df[i] = df[i].map(lambda x: (x - des[i]['mean']) / des[i]['std'])

dfl = (dfl - dfl.mean()) / dfl.std()

df.to_csv("data.csv")
dfl.to_csv("label.csv")