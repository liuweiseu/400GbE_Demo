#! /bin/bash

cpus=`cat /proc/cpuinfo| grep "processor"| wc -l`

if [ ! $1 ] ;then
	echo "Please specify the cpu mode: performance or powersave"
	exit
fi

if [ $1 == 'performance' ] ;then
	echo "Setting cpu to performance mode."
	for((i=0;i<${cpus};i++));
    	do
            cpufreq-set -c $i -g performance;
    	done
	echo "Done."
fi

if [ $1 == 'powersave' ] ;then
	echo "Setting cpu to powersave mode."
        for((i=0;i<${cpus};i++));
        do
            cpufreq-set -c $i -g powersave;
        done	
	echo "Done."
fi

if [ $1 == 'check' ] ;then
        for((i=0;i<${cpus};i++));
        do
            freq=`cpufreq-info -c $i |grep "current CPU frequency"`
	    echo CPU$i: $freq
        done
fi
