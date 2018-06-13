#!/bin/bash

if [ -z $ADTNPLUS_ADAPTER_HOME ]; then
	echo "Error: \$ADTNPLUS_ADAPTER_HOME is not defined."
	exit
fi 

if [ -z $ADTNPLUS_HOME ]; then
	echo "Error: \$ADTNPLUS_HOME is not defined."
	exit
fi

if [ -z $ADTNPLUS_CONF ]; then
	echo "Error: \$ADTNPLUS_CONF is not defined."
	exit
fi

# ----------------------------------------------------------
# Variables used by LEPTON
oppnet_adapter_classname=uab.senda.lepton.hub.AdtnPlus_Adapter
oppnet_adapter_classpath="${ADTNPLUS_ADAPTER_HOME}/libs/adtnPlus-adapter-1.0.jar"
node_process_tag=adtn

# ----------------------------------------------------------
# Variables used by aDTN nodes

if [ "$lepton_host" == "" ] || [ "$lepton_host" == "localhost" ] ; then
    lepton_host=127.0.0.1
fi

ip_host=$(ip a s $(ip link  | grep -v -E "loopback|LOOPBACK" | head -1 | cut -d" " -f 2) | head -3 | tail -1 | cut -d" " -f 6 | cut -d"/" -f 1)

disc_addr=$lepton_host
disc_port=4500
start_port=38000

echo $start_port > /tmp/port_aux


# ------------------------------------------------------------
# Functions used by LEPTON: start_node() and stop_node() are inherited
# from ${DODWAN_HOME}/bin/util/ibrdtn_functions.sh
# ------------------------------------------------------------

. ${ADTNPLUS_ADAPTER_HOME}/bin/utils/adtn_functions.sh

# ----------------------------------------------------------
exec_on_node() {
    echo "Executing on ${node_id}: adtn.sh $*"
    node_id=$node_id $ADTNPLUS_ADAPTER_HOME/bin/adtn.sh "$@"
}
