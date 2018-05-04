from rabaDB.rabaSetup import *
import rabaDB.Raba as R
import rabaDB.fields as rf
from rabaDB.filters import *
from numpy import mean, median

# Globals
smallestTimestamp = sys.maxint
info = dict()
info['nodes'] = list()
names_lookup = dict()


class Entry(R.Raba):
    _raba_namespace = 'statistics'

    type_id = rf.Primitive()
    timestamp = rf.Primitive()
    bundle_id = rf.Primitive()
    to_id = rf.Primitive()
    from_id = rf.Primitive()
    file_from = rf.Primitive()
    timestamp_bundle = rf.Primitive()
    size_bundle = rf.Primitive()

    def __init__(self):
        pass


def parseFile(file):
    for line in file:
        splitted = line.split()
        entry = Entry()
        entry.file_from = file.name
        entry.type_id = splitted[0]
        entry.timestamp = int(splitted[1])
        # PLATFROM_START (0 UNIX_TIME NODE_ID)
        if splitted[0] == '0':
            entry.from_id = splitted[2]
            names_lookup[splitted[3]] = splitted[2]
        # PLATFROM_END (1 UNIX_TIME)
        elif splitted[0] == '1':
            pass
        # MESSAGE_CREATED (2 UNIX_TIME BUNDLE_ID)
        elif splitted[0] == '2':
            entry.bundle_id = splitted[2]
        # MESSAGE_RELAYED_FROM (3 UNIX_TIME BUNDLE_ID FROM_ID)
        elif splitted[0] == '3':
            entry.bundle_id = splitted[2]
            entry.from_id = splitted[3].split(":")[0]
        # MESSAGE_RELAYED (4 UNIX_TIME BUNDLE_ID TO_ID)
        elif splitted[0] == '4':
            entry.bundle_id = splitted[2]
            entry.to_id = splitted[3].split(":")[0]
        # MESSAGE_DELIVERED (5 UNIX_TIME)
        elif splitted[0] == '5':
            pass
        # MESSAGE_DROPPED (6 UNIX_TIME BUNDLE_ID)
        elif splitted[0] == '6':
            entry.bundle_id = splitted[2]
        # MESSAGE_REMOVED (7 UNIX_TIME BUNDLE_ID)
        elif splitted[0] == '7':
            entry.bundle_id = splitted[2]
        # MESSAGE_RECEIVED (8 UNIX_TIME BUNDLE_ID SIZE 2000_TIME)
        elif splitted[0] == '8':
            entry.bundle_id = splitted[2]
            entry.size_bundle = splitted[3]
            secondsTimestamp = int(splitted[4]) + 946684800
            entry.timestamp_bundle = secondsTimestamp * 1000
        # NEIGH_APPEAR (9 UNIX_TIME NEIGH_ID)
        elif splitted[0] == '9':
            entry.to_id = splitted[2]
        # NEGIH_DISAPPEAR (10 UNIX_TIME NEIGH_ID)
        elif splitted[0] == '10':
            entry.to_id = splitted[2]
        entry.save()


def parseFiles(files):
    conn = RabaConnection('statistics')
    conn.beginTransaction()
    for f in files:
        parseFile(f)
    for k, v in names_lookup.iteritems():
        f = RabaQuery(Entry)
        f.addFilter(to_id=k)
        p = f.run()
        for i in p:
            i.set(to_id=v)
            i.save()
        g = RabaQuery(Entry)
        g.addFilter(from_id=k)
        q = g.run()
        for y in q:
            y.set(from_id=v)
            y.save()
    conn.endTransaction()


def generateWindowValues(files, window):
    pass


def calculateBufferTime(name):
    times = []
    processedID = []
    f = RabaQuery(Entry)
    f.addFilter(type_id="2", file_from=name)
    f.addFilter(type_id="3", file_from=name)
    for e in f.iterRun():
        q = RabaQuery(Entry)
        q.addFilter(type_id="7", file_from=name, bundle_id=e.bundle_id)
        p = q.run()
        aux = [x for x in p if x.raba_id not in processedID]
        if (len(aux) > 0):
            times.append(aux[0].timestamp - e.timestamp)
            processedID.append(aux[0].raba_id)
    return (median(times), mean(times))


def getBytes(name):
    q = RabaQuery(Entry)
    q.addFilter(type_id="8", file_from=name)
    nBytes = 0
    for e in q.iterRun():
        nBytes += int(e.size_bundle)
    return nBytes


def calculateReceivedInfo(name):
    copies = []
    times = []
    q = RabaQuery(Entry)
    q.addFilter(type_id="8", file_from=name)
    alreadyProcessed = set()
    for e in q.iterRun():
        if e.bundle_id not in alreadyProcessed:
            f = RabaQuery(Entry)
            f.addFilter(type_id="8", file_from=name, bundle_id=e.bundle_id)
            copies.append(f.count())
            alreadyProcessed.add(e.bundle_id)
        times.append(e.timestamp - e.timestamp_bundle)
    if len(copies) > 0:
        return (median(copies), mean(copies), median(times), mean(times))
    else:
        if len(times) > 0:
            return (0, 0, median(times), mean(times))
        else:
            return (0, 0, 0, 0)


def getBytesDelivered(name):
    nBytes = 0
    q = RabaQuery(Entry)
    q.addFilter(type_id="4", file_from=name)
    for e in q.iterRun():
        f = RabaQuery(Entry)
        f.addFilter(type_id="3", bundle_id=e.bundle_id, from_id=name)
        p = f.run()
        if (len(p) > 0):
            g = RabaQuery(Entry)
            g.addFilter(type_id="8", bundle_id=e.bundle_id,
                        file_from=p[0].file_from)
            p1 = g.run()
            if (len(p1) > 0):
                nBytes += int(p1[0].size_bundle)
    return nBytes


def getNeighbourInfo(name):
    q = RabaQuery(Entry)
    q.addFilter(type_id="9", file_from=name)
    contacts = q.count()
    times = []
    processedID = []
    f = RabaQuery(Entry)
    f.addFilter(type_id="9", file_from=name)
    for e in f.iterRun():
        q = RabaQuery(Entry)
        q.addFilter(type_id="10", file_from=name, to_id=e.to_id)
        p = q.run()
        aux = [x for x in p if x.raba_id not in processedID]
        if (len(aux) > 0):
            times.append(aux[0].timestamp - e.timestamp)
            processedID.append(aux[0].raba_id)
    return (contacts, median(times), mean(times))


def generateJSON(files, window):
    for f in files:
        # Generate the node dictionary
        info_node = {"node_id": f.name, "message": dict(), "node": dict()}
        # Look for the different entries
        # MESSAGE_CREATED (2)
        q = RabaQuery(Entry)
        q.addFilter(type_id="2", file_from=f.name)
        info_node["message"]["created"] = q.count()
        # MESSAGE_RELAYED_FROM (3)
        q = RabaQuery(Entry)
        q.addFilter(type_id="3", file_from=f.name)
        info_node["message"]["relayed_from"] = q.count()
        # MESSAGE_RELAYED (4)
        q = RabaQuery(Entry)
        q.addFilter(type_id="4", file_from=f.name)
        info_node["message"]["relayed"] = q.count()
        # MESSAGE_DELIVERED (5) (bytes)
        info_node["message"]["delivered"] = getBytesDelivered(f.name)
        # MESSAGE_DROPPED (6)
        q = RabaQuery(Entry)
        q.addFilter(type_id="6", file_from=f.name)
        info_node["message"]["dropped"] = q.count()
        # MESSAGE_RECEIVED (8) (bytes)
        info_node["message"]["received"] = getBytes(f.name)
        receivedInfo = calculateReceivedInfo(f.name)
        info_node["message"]["CopiesReceivedPerMsg_med"] = receivedInfo[0]
        info_node["message"]["CopiesReceivedPerMsg_avg"] = receivedInfo[1]
        info_node["message"]["lifeTimeReceived_med"] = receivedInfo[2] / 1000
        info_node["message"]["lifeTimeReceived_avg"] = receivedInfo[3] / 1000
        bufferTime = calculateBufferTime(f.name)
        info_node["message"]["bufferTime_med"] = bufferTime[0] / 1000
        info_node["message"]["bufferTime_avg"] = bufferTime[1] / 1000
        try:
            overhead_ratio = (info_node["message"]["received"] -
                              info_node["message"]["delivered"]) / info_node[
                                  "message"]["delivered"]
        except ZeroDivisionError:
            overhead_ratio = 'NaN'
        info_node["message"]["overhead_ratio"] = overhead_ratio
        contactInfo = getNeighbourInfo(f.name)
        info_node["node"]["contacts"] = contactInfo[0]
        info_node["node"]["contactTime_med"] = contactInfo[1] / 1000
        info_node["node"]["contactTime_avg"] = contactInfo[2] / 1000

        # Append the node
        info["nodes"].append(info_node)
    aggr = {"message": dict(), "node": dict()}
    aggr["message"]["created"] = sum(x["message"]["created"] for x in info["nodes"])
    aggr["message"]["relayed"] = sum(x["message"]["relayed"] for x in info["nodes"])
    aggr["message"]["relayed_from"] = sum(x["message"]["relayed_from"] for x in info["nodes"])
    aggr["message"]["received"] = sum(x["message"]["received"] for x in info["nodes"])
    aggr["message"]["delivered"] = sum(x["message"]["delivered"] for x in info["nodes"])
    overhead_ratio = (aggr["message"]["received"] - aggr["message"]["delivered"]) / aggr[
                                  "message"]["delivered"]
    aggr["message"]["CopiesReceivedPerMsg_avg"] = mean([x["message"]["CopiesReceivedPerMsg_avg"] for x in info["nodes"] if x["message"]["CopiesReceivedPerMsg_avg"] is not 0])
    aggr["message"]["CopiesReceivedPerMsg_med"] = median([x["message"]["CopiesReceivedPerMsg_med"] for x in info["nodes"] if x["message"]["CopiesReceivedPerMsg_med"] is not 0])
    info["aggregated"] = aggr
    with open('data.txt', 'w') as outfile:
        json.dump(info, outfile)
