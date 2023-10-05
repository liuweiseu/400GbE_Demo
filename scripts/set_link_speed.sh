#! /bin/bash

if [ ! $1 ] ;then
	echo "Please specify link speed: 200g or 400g"
	exit
fi

if [ $1 == '200g' ] ;then
    ethtool -s ens6np0 speed 200000 autoneg on
    sleep 1
    mlxlink -d mlx5_0
fi

if [ $1 == '400g' ] ;then
    ethtool -s ens6np0 speed 400000 autoneg on
    sleep 1
    mlxlink -d mlx5_0
fi