#! /bin/bash

if [ ! $1 ] ;then
	echo "Please specify the PCIe address of the NIC!"
	exit
fi

service opensmd stop
mstconfig -d $1 set LINK_TYPE_P1=2
mstfwreset -d $1 -l3 -y reset
