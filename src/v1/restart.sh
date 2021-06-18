#!/bin/bash

if [ $# != 4 ] ; then
	echo ""
	echo "Uasge:"
	echo "sh $0 host start_port path node_count"
	echo "sh $0 127.0.0.1 38000 /tmp/vpaxos 5"
	echo ""
	exit 1
fi

host=$1
start_port=$2
path=$3
node_count=$4
	
#echo  ==================
#echo ${host}
#echo ${start_port}
#echo ${path}
#echo ${node_count}
#echo  ==================

killall vpaxos_server
sleep 1
mkdir -p /tmp/vpaxos
rm -rf /tmp/vpaxos/*


echo "${host} ${start_port} ${path} ${node_count}" | awk '{
	host = $1
	start_port = $2
	path = $3
	node_count = $4
} END {
	all_addr=""
	for (i = 0; i < node_count; i++) {
		port = start_port + i
		addr = host":"port
		addrs[i] = addr
		hosts[i] = host
		ports[i] = port
		all_addr=all_addr","addrs[i]
	}

	for (i in addrs) {
		#print addrs[i], hosts[i], ports[i]
	
		peers=all_addr
		sub(","addrs[i], "", peers)
		peers=substr(peers, 2)

		print "nohup ./vpaxos_server --me="addrs[i]" --peers="peers" --path="path"/"addrs[i]" 1>"ports[i]".msg 2>"ports[i]".err &"
	}
}' > tmp_start.sh

sh tmp_start.sh
rm tmp_start.sh



exit 0
