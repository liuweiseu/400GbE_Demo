##########################################################################
# File Name: bw_test.sh
# Author: Wei Liu
# mail: liuwei_berkeley@berkeley.edu
# Created Time: Wed 18 Oct 2023 01:32:27 PM UTC
#########################################################################
#!/bin/bash

dev=mlx5_0
dur=10

if [ ! $1 ] ;then
	echo "Please specify the packet type: --ib or --eth"
	exit
fi

if [ $1 == '--ib' ] ;then
	if [ ! $2 ] ;then
		echo "Server Mode - IB Type"
		ib_send_bw -d $dev -D $dur --report_gbits
	else
		echo "Client Mode - IB Type"
		ib_send_bw $2 -d $dev -D $dur --report_gbits
	fi
fi
