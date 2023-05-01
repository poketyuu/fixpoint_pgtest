import random
from random import randint
from datetime import datetime,timedelta
import pandas as pd

def getdatetimestring(x:datetime) -> str:
    return x.strftime('%Y%m%d%H%M%S')

def getping() -> str:
    x = random.random()
    if x < 0.8:
        return str(randint(1,1000))
    else:
        return "-"
    
def getrandomipaddress(net:int,host:int):
    adlist = []
    netadlist = []
    while len(netadlist) < net:
        prefix = randint(1, 28)
        netaddress = randint(0, 2 ** prefix - 1)
        if netaddress in netadlist:
            continue
        netadlist.append(netaddress)
        hostadlist = []
        while len(hostadlist) < host:
            hostaddress = randint(0, 2 ** (32-prefix) -1)
            if hostaddress in hostadlist:
                continue
            hostadlist.append(hostaddress)
            address = netaddress * (2 ** (32-prefix)) + hostaddress
            s = ""
            for i in range(4):
                num = 255 & (address >> (8 * i))
                s = str(num) + s
                if i != 3:
                    s = "." + s
            s = s + "/" + str(prefix)
            adlist.append(s)
    return adlist


random.seed()

N = 100000
date = datetime.now()
duration = 300

timing = []
while len(timing) < 100:
    t = randint(0,duration-1)
    if t in timing:
        continue
    timing.append(t)
timing.sort()

print(len(timing))

datelog = []

for i in range(1000):
    for j in range (100):
        datelog.append(getdatetimestring(
            date + timedelta(seconds=(timing[j] + duration * i))))

addresslist = getrandomipaddress(50,2)
addresslog = []

for i in range(1000):
    for j in range(100):
        addresslog.append(addresslist[j])

pd_date_log = pd.Series(datelog)

pd_address_log = pd.Series(addresslog)

str_ping_list = pd.Series(
    [getping() for _ in range(N)]
)

dfm_log = pd.concat(
    [pd_date_log,pd_address_log,str_ping_list],axis=1
)
dfm_log.to_csv('testdata2.csv',index=False,header=False)