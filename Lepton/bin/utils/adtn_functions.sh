# ----------------------------------------------------------
init_dirs () {
	base_dir=/run/shm/${USER}/adtn
	node_dir=${base_dir}/${node_id}
	pid_file=${node_dir}/pid
	conf_file=${node_dir}/adtn.ini
	node_file=${node_dir}/NodeState.json
}

# ----------------------------------------------------------
init_vars () {
	# -------------------
	# Variables for the configuration
	log_level=21
	queue_size="1M"
	# -------------------
	START_PORT_ACCEPT=$(cat /tmp/port_aux)
	PORT_ACCEPT=${START_PORT_ACCEPT}
	((++START_PORT_ACCEPT))
	PORT_RECV=${START_PORT_ACCEPT}
	((++START_PORT_ACCEPT))
	echo $START_PORT_ACCEPT > /tmp/port_aux
}

. ${ADTNPLUS_ADAPTER_HOME}/bin/utils/conf_functions.sh

# ----------------------------------------------------------
# Define where to find aDTNPlus 
adtnd=${ADTNPLUS_HOME}/bin/BundleAgent
adtnsend=${ADTNPLUS_HOME}/bin/adtnPlus-sender
adtnrecv=${ADTNPLUS_HOME}/bin/adtnPlus-recv

# ------------------------------------------------------------
start_node() {
	#
    # Starts an IBRDTN node
    #
    # Uses the following variables:
    #
    # node_id        : id of the node (required)
    # node_start_time: time when the node should start (EPOCH in ms, OPTIONAL)
    # node_end_time  : time when the node should stop (EPOCH in ms, OPTIONAL)
    # node_seed      : seed to be used by that node's random generator (OPTIONAL)
    # itf            : interface the IBRDTN node should bind to (required)
    # disc_addr      : address to use for neighbor discovery (required)
    # disc_port      : port number to use for neighbor discovery (required)
    # disc_lport     : local port number to use for neighbor discovery (OPTIONAL)
 
 	echo Starting node $node_id

 	init_dirs
 	init_vars

 	if [ ! -d $node_dir ]; then
 		mkdir -p $node_dir
 	fi
 	if [ ! -d $node_dir/Bundles ]; then
 		mkdir -p $node_dir/Bundles
 	fi
 	if [ ! -d $node_dir/Delivered ]; then
 		mkdir -p $node_dir/Delivered
 	fi
 	if [ ! -d $node_dir/Codes ]; then
 		mkdir -p $node_dir/Codes
 	fi
 	if [ ! -d $node_dir/Plugins ]; then
 		mkdir -p $node_dir/Plugins
 		cp $ADTNPLUS_CONF/Plugins/* $node_dir/Plugins/
 	fi
 	if [ ! -d $node_dir/Trash/aggregation/reception ]; then
 		mkdir -p $node_dir/Trash/aggregation/reception
 	fi
 	if [ ! -d $node_dir/Trash/aggregation/delivery ]; then
 		mkdir -p $node_dir/Trash/aggregation/delivery
 	fi
 	if [ ! -d $node_dir/Trash/drop ]; then
 		mkdir -p $node_dir/Trash/drop
 	fi

 	gen_conf_file
 	gen_nodeState_file

    $adtnd ${conf_file} &
  	echo $! > $pid_file
}


# ------------------------------------------------------------
stop_node() {
	#
    # Stops an aDTN node
    #
    # Uses the following variables:
    #
    # node_id      : id of the node (required)
    
    echo Stopping node $node_id

    init_dirs

    check_running

  	pid=$(cat $pid_file)
 	kill $pid >& /dev/null
  	rm -f $pid_file
}

# ------------------------------------------------------------
check_running() {
    
    if [ ! -e ${pid_file} ] ; then
	echo "Error: it seems node ${node_id} is not running"
	exit 1
    fi
}

# ------------------------------------------------------------
status() {
    #
    # Shows the status of an aDTN node (i.e. running or not running)
    #
    # node_id: id of the node (required)

    init_dirs

    if [ -e ${pid_file} ] ; then
	echo "Node ${node_id} is running"
    else
	echo "Node ${node_id} is not running"
    fi
}

# ------------------------------------------------------------
send() {
    #
    # Transfers a file between two aDTN nodes
    #
    # node_id: id of the source node (required)
    # $1     : id of the destination node (required)
    # $2     : message to be send (required)

    dst=$1
    message=${*:2:$#}

    init_dirs

    check_running

    ip_src=$(cat $conf_file | grep nodeAddress | cut -d" " -f 3)
    port_src=$(cat $conf_file | grep nodePort | cut -d" " -f 3)

    $adtnsend -i $ip_src -p $port_src -d $dst -m "$message"
}

# ------------------------------------------------------------
recv() {
    #
    # Receives files from other aDTN nodes
    #
    # node_id: id of the node that should receive messages (required)

    init_dirs

    check_running

    ip_listen=$(cat $conf_file | grep listenerAddress | cut -d" " -f 3)
    port_listen=$(cat $conf_file | grep listenerPort | cut -d" " -f 3)
    
    $adtnrecv -i $ip_listen -p $port_listen -a $node_id
}


