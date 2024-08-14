##########################################################################
# File Name: smt.sh
# Author: Wei Liu
# mail: liuwei_berkeley@berkeley.edu
# Created Time: Mon 23 Oct 2023 07:23:46 AM UTC
#########################################################################
#!/bin/bash

if [ ! $1 ] ;then
	echo "Please specify the smt status: on or off"
	exit
fi

if [ $1 == '--on' ] ;then
	bash -c "echo 'on' > /sys/devices/system/cpu/smt/control"
fi

if [ $1 == '--off' ] ;then
	bash -c "echo 'off' > /sys/devices/system/cpu/smt/control"
fi

if [ $1 == '--check' ] ;then
	echo 'smt status:' `cat /sys/devices/system/cpu/smt/control`
fi
