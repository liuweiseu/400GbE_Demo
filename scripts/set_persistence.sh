##########################################################################
# File Name: set_persistence.sh
# Author: Wei Liu
# mail: liuwei_berkeley@berkeley.edu
# Created Time: Tue 31 Oct 2023 05:35:56 AM UTC
#########################################################################
#!/bin/bash

if [ ! $1 ] ;then
	echo "Please specify the mode: --on, --off or --check"
	exit
fi

if [ $1 == '--on' ] ;then
	nvidia-smi --persistence-mode=ENABLED
fi

if [ $1 == '--OFF' ] ;then
	nvidia-smi --persistence-mode=DISABLED
fi

if [ $1 == '--check' ] ;then
	nvidia-smi -q |grep Persistence
fi
