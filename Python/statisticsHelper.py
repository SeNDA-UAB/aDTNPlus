from rabaDB.rabaSetup import *
import rabaDB.Raba as R
import rabaDB.fields as rf
from rabaDB.filters import *
from numpy import mean, median, bincount
from collections import defaultdict

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
        # MESSAGE_RELAYED_FROM (3 UNIX_TIME BUNDLE_ID FROM_ID SIZE)
        elif splitted[0] == '3':
            entry.bundle_id = splitted[2]
            entry.from_id = splitted[3].split(":")[0]
            # entry.from_id = int(splitted[4])
        # MESSAGE_RELAYED (4 UNIX_TIME BUNDLE_ID TO_ID SIZE)
        elif splitted[0] == '4':
            entry.bundle_id = splitted[2]
            entry.to_id = splitted[3].split(":")[0]
            entry.size_bundle = int(splitted[4])
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
            entry.size_bundle = int(splitted[3])
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
        nBytes += e.size_bundle
    return nBytes


def calculateReceivedInfo(name):
    copies = []
    times = []
    count = 0
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
        count += 1
    if len(copies) > 0:
        return (median(copies), mean(copies), median(times), mean(times),
                count, len(alreadyProcessed))
    else:
        if len(times) > 0:
            return (0, 0, median(times), mean(times),
                    count, len(alreadyProcessed))
        else:
            return (0, 0, 0, 0, count, len(alreadyProcessed))


def getBytesDelivered(name):
    nBytes = 0
    q = RabaQuery(Entry)
    q.addFilter(type_id="4", file_from=name)
    count = 0
    for e in q.iterRun():
        f = RabaQuery(Entry)
        f.addFilter(type_id="8", bundle_id=e.bundle_id, file_from=e.to_id)
        p = f.run()
        for n in p:
            g = RabaQuery(Entry)
            g.addFilter({"timestamp <=": n.timestamp, "type_id": "3",
                         "bundle_id": e.bundle_id, "file_from": n.file_from})
            p1 = g.run(sqlTail="ORDER By timestamp DESC")
            if (len(p1) > 0):
                if (p1[0].from_id == name):
                    nBytes += e.size_bundle
                    count += 1
    return (nBytes, count)


def getNeighbourInfo(name):
    q = RabaQuery(Entry)
    q.addFilter(type_id="9", file_from=name)
    contacts = q.count()
    times = []
    processedID = []
    times_dist = defaultdict(int)
    f = RabaQuery(Entry)
    f.addFilter(type_id="9", file_from=name)
    for e in f.iterRun():
        q = RabaQuery(Entry)
        q.addFilter(type_id="10", file_from=name, to_id=e.to_id)
        p = q.run()
        aux = [x for x in p if x.raba_id not in processedID]
        if (len(aux) > 0):
            diff = aux[0].timestamp - e.timestamp
            times.append(diff)
            processedID.append(aux[0].raba_id)
            times_dist[aux[0].to_id] += diff / 1000
    return (contacts, median(times), mean(times), times_dist)


def getContactsInfo(name):
    q = RabaQuery(Entry)
    q.addFilter(type_id="9", file_from=name)
    p = q.run()
    uniqueContacts = set(x.to_id for x in p)
    contactBetw = dict()
    for e in uniqueContacts:
        f = RabaQuery(Entry)
        f.addFilter(type_id="9", file_from=name, to_id=e)
        contactBetw[e] = f.count()
    dist = [y for x, y in contactBetw.iteritems()]
    dist = bincount(dist)
    contactDist = {x: dist[x] for x in range(1, len(dist)) if dist[x] != 0}
    return (contactBetw, contactDist)


def getBytesContact(name):
    q = RabaQuery(Entry)
    q.addFilter(type_id="4", file_from=name)
    send_bytes = defaultdict(int)
    recv_bytes = defaultdict(int)
    for e in q.iterRun():
        send_bytes[e.to_id] += e.size_bundle
    bytes_send = [v for k, v in send_bytes.iteritems()]
    if (len(bytes_send) > 0):
        return (median(bytes_send), mean(bytes_send))
    return (0, 0, 0, 0)


def hopPath(info):
    conn = RabaConnection('statistics')
    # conn.enableQueryPrint(True)
    hops = 0
    q = RabaQuery(Entry)
    res = q.runWhere("type_id = ? and file_from = ? and bundle_id = ? and timestamp / 1000 <= ? ORDER BY timestamp DESC",
                     ("3", info.file_from, info.bundle_id, info.timestamp / 1000))
    while(len(res) > 0):
        n = res[0]
        p = RabaQuery(Entry)
        # p.addFilter({"type_id": "4", "bundle_id": n.bundle_id,
        #             "file_from": n.from_id, "to_id": n.file_from,
        #             "timestamp / 1000 <=": n.timestamp / 1000})
        #res = p.run(sqlTail="ORDER By timestamp DESC")
        res = p.runWhere("type_id = ? and bundle_id = ? and file_from = ? and to_id = ? and timestamp / 1000 <= ? ORDER BY timestamp DESC",
                         ("4", n.bundle_id, n.from_id, n.file_from, n.timestamp / 1000))
        if (len(res) > 0):
            n1 = res[0]
            r = RabaQuery(Entry)
            # r.addFilter({"type_id": "3", "bundle_id": n.bundle_id,
            #             "file_from": n1.file_from,
            #             "timestamp / 1000 <=": n1.timestamp / 1000})
            #res = r.run(sqlTail="ORDER By timestamp DESC")
            res = r.runWhere("type_id = ? and bundle_id = ? and file_from = ? and timestamp / 1000 <= ? ORDER BY timestamp DESC",
                             ("3", n.bundle_id, n1.file_from, n1.timestamp / 1000))
            hops += 1
    # conn.enableQueryPrint(False)
    return hops


def hopCount():
    q = RabaQuery(Entry)
    q.addFilter(type_id="8")
    for e in q.iterRun(sqlTail="ORDER By timestamp DESC"):
        print hopPath(e)


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
        deliveredInfo = getBytesDelivered(f.name)
        info_node["message"]["delivered_bytes"] = deliveredInfo[0]
        info_node["message"]["delivered"] = deliveredInfo[1]
        # MESSAGE_DROPPED (6)
        q = RabaQuery(Entry)
        q.addFilter(type_id="6", file_from=f.name)
        info_node["message"]["dropped"] = q.count()
        # MESSAGE_RECEIVED (8) (bytes)
        info_node["message"]["received_bytes"] = getBytes(f.name)
        receivedInfo = calculateReceivedInfo(f.name)
        info_node["message"]["CopiesReceivedPerMsg_med"] = receivedInfo[0]
        info_node["message"]["CopiesReceivedPerMsg_avg"] = receivedInfo[1]
        info_node["message"]["lifeTimeReceived_med"] = receivedInfo[2] / 1000
        info_node["message"]["lifeTimeReceived_avg"] = receivedInfo[3] / 1000
        info_node["message"]["received_total"] = receivedInfo[4]
        info_node["message"]["received_unique"] = receivedInfo[5]
        bufferTime = calculateBufferTime(f.name)
        info_node["message"]["bufferTime_med"] = bufferTime[0] / 1000
        info_node["message"]["bufferTime_avg"] = bufferTime[1] / 1000
        try:
            overhead_ratio = (info_node["message"]["received_bytes"] -
                              info_node["message"]["delivered_bytes"]) \
                / info_node["message"]["delivered_bytes"]
        except ZeroDivisionError:
            overhead_ratio = 'NaN'
        info_node["message"]["overhead_ratio_bytes"] = overhead_ratio
        contactInfo = getNeighbourInfo(f.name)
        info_node["node"]["contacts"] = contactInfo[0]
        info_node["node"]["contactTime_med"] = contactInfo[1] / 1000
        info_node["node"]["contactTime_avg"] = contactInfo[2] / 1000
        info_node["node"]["totalContactTime_dist"] = contactInfo[3]
        contactDists = getContactsInfo(f.name)
        info_node["node"]["nrOfContactsBetweenNodes_dist"] = contactDists[0]
        info_node["node"]["nrOfContacts_dist"] = contactDists[1]
        bytesDists = getBytesContact(f.name)
        info_node["node"]["bytesSendPercontact_med"] = bytesDists[0]
        info_node["node"]["bytesSendPercontact_avg"] = bytesDists[1]

        # Append the node
        info["nodes"].append(info_node)
    aggr = {"message": dict(), "node": dict()}
    aggr["message"]["created"] = sum(
        x["message"]["created"] for x in info["nodes"])
    aggr["message"]["relayed"] = sum(
        x["message"]["relayed"] for x in info["nodes"])
    aggr["message"]["relayed_from"] = sum(
        x["message"]["relayed_from"] for x in info["nodes"])
    aggr["message"]["received_bytes"] = sum(
        x["message"]["received_bytes"] for x in info["nodes"])
    aggr["message"]["delivered_bytes"] = sum(
        x["message"]["delivered_bytes"] for x in info["nodes"])
    overhead_ratio = (aggr["message"]["received_bytes"] - aggr["message"]["delivered_bytes"]) / aggr[
        "message"]["delivered_bytes"]
    aggr["message"]["CopiesReceivedPerMsg_avg"] = mean([x["message"]["CopiesReceivedPerMsg_avg"] for x in info[
                                                       "nodes"] if x["message"]["CopiesReceivedPerMsg_avg"] is not 0])
    aggr["message"]["CopiesReceivedPerMsg_med"] = median([x["message"]["CopiesReceivedPerMsg_med"] for x in info[
                                                         "nodes"] if x["message"]["CopiesReceivedPerMsg_med"] is not 0])
    info["aggregated"] = aggr
    print hopCount()
    with open('data.txt', 'w') as outfile:
        json.dump(info, outfile)
