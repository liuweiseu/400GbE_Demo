##########################################################################
# File Name: set_gpu_clock.sh
# Author: Wei Liu
# mail: liuwei_berkeley@berkeley.edu
# Created Time: Thu 09 Nov 2023 11:41:46 PM UTC
#########################################################################
#!/bin/bash

if [ ! $1 ] ;then
	echo "Please specify the clock mode: --performance or --powersave"
	exit
fi

if [ $1 == '--performance' ] ;then
    nvidia-smi -lmc=8001,8001
	echo 'Done.'
fi

if [ $1 == '--powersave' ] ;then
	nvidia-smi -rmc
	echo 'Done.'
fi
