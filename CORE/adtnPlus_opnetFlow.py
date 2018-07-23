#
# CORE
# Copyright (c)2010-2012 the Boeing Company.
# See the LICENSE file included in this distribution.
#
''' aDTNPlus user-defined service.
'''

from core.service import CoreService, addservice
from core.misc.utils import *
from core.constants import *

class AdtnPlus(CoreService):
    '''This is the aDTNPlus user-defined service.
    '''
    # a unique name is required, without spaces
    _name = "aDTNPlus"
    # you can create your own group here
    _group = "Utility"
    # list of other services this service depends on
    _depends = ()
    # per-node directories
    _dirs = ()
    # generated files (without a full path this file goes in the node's dir,
    #  e.g. /tmp/pycore.12345/n1.conf/)
    _configs = ('adtnPlus/adtn.ini', 'adtnPlus/NodeState.json', 'plugins.sh',
                'adtnPlus-startup.sh', 'adtnPlus-shutdown.sh',)
    # this controls the starting order vs other enabled services
    _startindex = 50
    # list of startup commands, also may be generated during startup
    _startup = ('sh plugins.sh', 'sh adtnPlus-startup.sh',)
    # list of shutdown commands
    _shutdown = ('sh adtnPlus-shutdown',)

    @classmethod
    def generateconfig(cls, node, filename, services):
        '''This method calls the functions for every defined config file.
        '''
        if filename == "adtnPlus/adtn.ini":
            return cls.generateconfigadtn(node, filename, services)
        elif filename == "adtnPlus/NodeState.json":
            return cls.generatenodeState(node, filename, services)
        elif filename == "plugins.sh":
            return cls.generatestartupplugin(node, filename, services)
        elif filename == "adtnPlus-startup.sh":
            return cls.generatestartupadtnstartup(node, filename, services)
        elif filename == "adtnPlus-shutdown.sh":
            return cls.generateshutdownadtnshutdown(node, filename, services)
        else:
            return ""

    @classmethod
    def generateconfigadtn(cls, node, filename, services):
        for ifc in node.netifs():
                nodeip = ifc.addrlist[0].split("/")[0]
                break
        return """\
# This file contains the configuration of the aDTNPlus.

[Node]
# Node identification name.
nodeId : %s
# IP of this node to receive bundles.
nodeAddress : %s
# Port of this node to receive bundles.
nodePort : 40000
# Clean the previous bundles
clean : false

[NeighbourDiscovery]
# IP address to send the beacons.
discoveryAddress : 239.100.100.100
# Port to send the beacons.
discoveryPort : 40001
# Interval time to send the beacons in seconds.
discoveryPeriod : 2
# Neighbour Expiration time in seconds.
neighbourExpirationTime : 4
# Neighbour Cleaner waiting time in seconds.
neighbourCleanerTime : 2
# Test mode, in this mode the neighbour discovery takes our beacons as a neighbour
# allowing to send and receiver beacons and bundles to ourselves.
testMode : false

[Logger]
# File to save the logs.
filename : %s/adtnPlus/adtn.log
# Level of log, 1 to 100, 1 only errors 100 all log.
# Level 1 is errors only
# Level 6 is errors and warnings
# Level 21 is errors, warnings and information.
# Up to this level are debug logs.
level : 68

[Constants]
# Socket timeout in seconds.
timeout : 10
# Queue max size it bytes, K M and G can be used to express KB, MB and GB
#queueByteSize : 100M
queueByteSize : 1M

[BundleProcess]
# Path to save the bundles, it has to exist and the application has to have
# permissions in the folder.
dataPath : %s/adtnPlus/Bundles/
# Bundle process shared library name to use in this node.
bundleProcessName : %s/adtnPlus/Plugins/libaDTNPlus_OppnetFlowBundleProcessor.so
# Path to save the generated codes, it has to exist and the application has to
# have permissions in the folder.
codePath : %s/adtnPlus/Codes/
# Path to save the not correctly delivered bundles.
deliveryPath : %s/adtnPlus/Delivered/
# Path to save the trashed bundles when aggregating in the node.
trashAggregationReception : %s/adtnPlus/Trash/aggregation/reception/
# Path to save the trashed bundles when aggregating in the delivery.
trashAggregationDelivery : %s/adtnPlus/Trash/aggregation/delivery/
# Path to save the trashed bundles when dropped by full queue.
trashDropp : %s/adtnPlus/Trash/drop/

[AppListener]
# IP address to listen
listenerAddress : %s
# Port to register apps
listenerPort : 50000

[NodeState]
# Default node state path
path : %s/adtnPlus/NodeState.json
""" % (node.name, nodeip, node.nodedir, node.nodedir,
            node.nodedir, node.nodedir, node.nodedir, node.nodedir,
            node.nodedir, node.nodedir, nodeip, node.nodedir)

    @classmethod
    def generatenodeState(cls, node, filename, services):
        controllerId = "n1"
        controllerIds = ["n1"]
        return r"""
{
  "configuration" : {
    "defaultCodes" : {
      "forwarding" : "if (bps[\"delivered\"]) {bps[\"discard\"] = true; return std::vector<std::string>();} else {auto neighbours = ns(\"eid.connected.all\"); std::vector<std::string> toSend = std::vector<std::string>(); if (neighbours.size() > 0) {int pos = rand() %% neighbours.size(); toSend.push_back(neighbours[pos]);}return toSend;}",
      "lifetime" : "uint64_t creationTimestamp = bs(\"timestamp.value\"); if(bs(\"lifetime\") < (time(NULL) - g_timeFrom2000 - creationTimestamp)) return true; else return false;",
      "destination" : "auto destination = bs(\"destination\"); auto endpoints = ns(\"eid.registered\"); if(std::find(endpoints.begin(), endpoints.end(), destination) != endpoints.end()) return std::vector<std::string>({destination}); else return std::vector<std::string>();",
      "creation" : "bps[\"delivered\"] = false; bps[\"discard\"] = false; bps[\"forwarded\"] = false;",
      "deletion" : ""
    },
    "logLevel" : 68
  },
  "state" : {
    "stop" : false,
    "changed" : false
  },
  "oppnetFlow" : {
      "active" : true,   
      "forwardingAlgorithm" : "sprayandwait",
      "sprayandwait" : {
        "nrofCopies" : 10,
        "binaryMode" : true
      },      
      "metrics" : {
        "congestionWindow" : 5
      },  
      "control" : {    
        "joinedAsAController" : %s,
        "controllersGroupId" : "%s",          
        "generateControlDirectives" : %s, 
        "executeControlDirectives" : %s,
        "controlReportings" : {
          "active" : %s,
          "frequency" : 5,
          "lastControlBundleId" : ""
        },
       "forwardPriorization": {
          "myCtrlDirective": true,
          "myCtrlMetric": true,
          "forwardedCtrlDirective": true,
          "forwardedMetricDirective": true
        }
      }           
  },
  "id" : "%s"
}
""" % ('true' if node.name in controllerIds else 'false', controllerId, 'true' if node.name in controllerIds else 'false', 'true', 'true', node.name)


    @classmethod
    def generatestartupplugin(cls, node, filename, services):
        return """\
#! /bin/bash
NODEDIR=%s
mkdir -p $NODEDIR/adtnPlus/Plugins

localPath=/var/lib/adtnPlus/Plugins/
if [ -d $localPath ]; then
    cp /var/lib/adtnPlus/Plugins/* $NODEDIR/adtnPlus/Plugins/
fi
""" % (node.nodedir)

    @classmethod
    def generatestartupadtnstartup(cls, node, filename, services):
        return """\
#! /bin/bash
# auto-generated by adtnPlus user-defined service.

# create data folder for aDTNPlus platform
mkdir -p adtnPlus/Bundles
mkdir -p adtnPlus/Delivered
mkdir -p adtnPlus/Codes
mkdir -p adtnPlus/Trash/aggregation/delivery
mkdir -p adtnPlus/Trash/aggregation/reception
mkdir -p adtnPlus/Trash/drop
CFGPATH=%s
sleep 2
# launch aDTNPlus platform service
BundleAgent $CFGPATH &
""" % (node.nodedir + "/adtnPlus/adtn.ini")

    @classmethod
    def generateshutdownadtnshutdown(cls, node, filename, services):
        return """\
#! /bin/bash
# auto-generated by adtnPlus user-defined service.

# stop aDTNPlus platform
pkill BundleAgent > /dev/null 2>&1 &

sleep 3
# delete data folder for aDTN platform
rm -fr adtnPlus
"""

# this line is required to add the above class to the list of available services
addservice(AdtnPlus)
