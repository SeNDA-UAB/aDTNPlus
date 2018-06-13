# ----------------------------------------------------------
gen_conf_file() {
	cat <<EOF | sed -e s%HOSTNAME%${node_id}%g \
			-e s%IP_%${ip_host}%g \
			-e s%NODE_PORT%${PORT_ACCEPT}%g \
			-e s%NODE_RECV%${PORT_RECV}%g \
			-e s%DISC_ADDR%${disc_addr}%g \
			-e s%DISC_PORT%${disc_port}%g \
			-e s%NODE_DIR%${node_dir}%g \
			-e s%LOG_LEVEL%${log_level}%g \
			-e s%QUEUE_SIZE%${queue_size}%g \
			> ${conf_file}
# This file contains the configuration of the aDTNPlus.

[Node]
# Node identification name.
nodeId : HOSTNAME
# IP of this node to receive bundles.
nodeAddress : IP_
# Port of this node to receive bundles. 
nodePort : NODE_PORT
# Clean the previous bundles
clean : false

[NeighbourDiscovery]
# IP address to send the beacons.
discoveryAddress : DISC_ADDR
# Port to send the beacons.
discoveryPort : DISC_PORT
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
filename : NODE_DIR/adtn.log
# Level of log, 1 to 100, 1 only errors 100 all log.
# Level 1 is errors only
# Level 6 is errors and warnings
# Level 21 is errors, warnings and information.
# Up to this level are debug logs.
level : LOG_LEVEL

[Constants]
# Socket timeout in seconds.
timeout : 10
# Queue max size it bytes, K M and G can be used to express KB, MB and GB
queueByteSize : QUEUE_SIZE
# Process timeout in seconds. If no events triggered the queue to process, it 
# will be processed after this timeout.
processTimeout : 10

[BundleProcess]
# Path to save the bundles, it has to exist and the application has to have 
# permissions in the folder.
dataPath : NODE_DIR/Bundles/
# Bundle process shared library name to use in this node.
bundleProcessName : NODE_DIR/Plugins/libaDTNPlus_FirstFwkBundleProcessor.so
# Path to save the generated codes, it has to exist and the application has to
# have permissions in the folder.
codePath : NODE_DIR/Codes/ 
# Path to save the not correctly delivered bundles.
deliveryPath : NODE_DIR/Delivered/
# Path to save the trashed bundles when aggregating in the node.
trashAggregationReception : NODE_DIR/Trash/aggregation/reception/
# Path to save the trashed bundles when aggregating in the delivery.
trashAggregationDelivery : NODE_DIR/Trash/aggregation/delivery/
# Path to save the trashed bundles when dropped by full queue.
trashDropp : NODE_DIR/Trash/drop/

[AppListener]
# IP address to listen
listenerAddress : 127.0.0.1
# Port to register apps
listenerPort : NODE_RECV

[NodeState]
# Default node state path
path : NODE_DIR/NodeState.json
EOF
}

gen_nodeState_file() {
	cat <<EOF | sed -e s%HOSTNAME%${node_id}%g \
			-e s%LOG_LEVEL%${log_level}%g \
			> ${node_file}
{
  "configuration" : {
    "defaultCodes" : {
      "forwarding" : "if (bps[\"delivered\"]) {bps[\"discard\"] = true; return std::vector<std::string>();} else {auto neighbours = ns(\"eid.connected.all\"); std::vector<std::string> toSend = std::vector<std::string>(); if (neighbours.size() > 0) {int pos = rand() % neighbours.size(); toSend.push_back(neighbours[pos]);}return toSend;}",
      "lifetime" : "uint64_t creationTimestamp = bs(\"timestamp.value\"); if(bs(\"lifetime\") < (time(NULL) - g_timeFrom2000 - creationTimestamp)) return true; else return false;",
      "destination" : "auto destination = bs(\"destination\"); auto endpoints = ns(\"eid.registered\"); if(std::find(endpoints.begin(), endpoints.end(), destination) != endpoints.end()) return std::vector<std::string>({destination}); else return std::vector<std::string>();",
      "creation" : "bps[\"delivered\"] = false; bps[\"discard\"] = false; bps[\"forwarded\"] = false;",
      "deletion" : ""
    },
    "logLevel" : LOG_LEVEL
  },
  "state" : {
    "stop" : false,
    "changed" : false
  },
  "id" : "HOSTNAME"
}
EOF
}