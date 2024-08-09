#! /bin/bash

numactl --physcpubind=4 RecvDemo -d 0 --smac a0:88:c2:0d:5e:28 --dmac 94:6d:ae:ac:f8:38 --sip 192.168.3.2 --dip 192.168.3.12 --sport 54278 --dport 54278 --gpu 1 2>> ibv_info-qp1.log