#! /bin/bash

if [ ! $1 ] ;then
	echo "Please specify the PCIe address of the NIC!"
	exit
fi

mstconfig -d 43:00.0 set LINK_TYPE_P1=1
mstfwreset -d 43:00.0 -l3 -y reset
service opensmd start
