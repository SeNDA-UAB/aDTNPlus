#!/bin/bash

if [ -z $ADTNPLUS_HOME ]; then
    echo "Error: \$ADTNPLUS_HOME is not defined."
    exit
fi

if [ -z $ADTNPLUS_ADAPTER_HOME ]; then
    echo "Error: \$ADTNPLUS_ADAPTER_HOME is not defined."
    exit
fi

. ${ADTNPLUS_ADAPTER_HOME}/bin/utils/adtn_functions.sh

# ------------------------------------------------------------
usage() {
    cat <<EOF

Usage: adtn.sh start
               | stop
               | status
               | send <dest> <fname>
               | recv

EOF
    exit 0
}

# ------------------------------------------------------------

if [ "$1" == "-h" ] ; then
    usage
fi

if [ -z $node_id ] ; then
    node_id=$HOSTNAME
fi


case $1 in
    start)
	start_node
	;;
    stop)
	stop_node
	;;
    status)
	status
	;;
    send)
	if [ $# -lt 2 ] ; then
	    usage
	fi
	shift 
	send $*
	;;
    recv)
	if [ $# -lt 1 ] ; then
	    usage
	fi
	shift
	recv $*
	;;
    *)
	usage
	;;
esac
