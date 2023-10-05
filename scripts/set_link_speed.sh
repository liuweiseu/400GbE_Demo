#! /bin/bash

if [ ! $1 ] ;then
	echo "Please specify link speed: --100g or --200g or --400g"
	exit
fi

if [ $1 == '--100g' ] ;then
    ethtool -s ens6np0 speed 100000 autoneg on
    sleep 1
    mlxlink -d mlx5_0 |grep "Speed       "
fi

if [ $1 == '--200g' ] ;then
    ethtool -s ens6np0 speed 200000 autoneg on
    sleep 1
    mlxlink -d mlx5_0 |grep "Speed       "
fi

if [ $1 == '--400g' ] ;then
    ethtool -s ens6np0 speed 400000 autoneg on
    sleep 1
    mlxlink -d mlx5_0 |grep "Speed       "
fi

if [ $1 == '--check' ] ;then
    mlxlink -d mlx5_0 |grep "Speed       "
fi