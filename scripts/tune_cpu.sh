#! /bin/bash

if [ ! $1 ] ;then
	echo "Please specify the cpu mode: performance or powersave"
	exit
fi

if [ $1 == 'performance' ] ;then
	echo "Setting cpu to performance mode."
	cpufreq-set -c 0 -g performance
	cpufreq-set -c 1 -g performance
	cpufreq-set -c 2 -g performance
	cpufreq-set -c 3 -g performance
	cpufreq-set -c 4 -g performance
	cpufreq-set -c 5 -g performance
	cpufreq-set -c 6 -g performance
	cpufreq-set -c 7 -g performance
	echo "Done."
fi

if [ $1 == 'powersave' ] ;then
	echo "setting cpu to powersave mode."
	cpufreq-set -c 0 -g powersave
	cpufreq-set -c 1 -g powersave
	cpufreq-set -c 2 -g powersave
	cpufreq-set -c 3 -g powersave
	cpufreq-set -c 4 -g powersave
	cpufreq-set -c 5 -g powersave
	cpufreq-set -c 6 -g powersave
	cpufreq-set -c 7 -g powersave
fi
