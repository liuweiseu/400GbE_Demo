#! /bin/bash

eth='ens4np0'
dev='mlx5_0'

if [ ! $1 ] ;then
	echo "Please specify link speed: --100g or --200g or --400g"
	exit
fi

if [ $1 == '--100g' ] ;then
    ethtool -s $eth speed 100000 autoneg on
    sleep 1
    mlxlink -d $dev |grep "Speed       "
fi

if [ $1 == '--200g' ] ;then
    ethtool -s $eth speed 200000 autoneg on
    sleep 1
    mlxlink -d $dev |grep "Speed       "
fi

if [ $1 == '--400g' ] ;then
    ethtool -s ${eth} speed 400000 autoneg on
    sleep 1
    mlxlink -d ${dev} |grep "Speed       "
fi

if [ $1 == '--check' ] ;then
    mlxlink -d $dev |grep "Speed       "
fi
