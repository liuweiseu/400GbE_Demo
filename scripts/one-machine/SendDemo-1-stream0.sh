#! /bin/bash

numactl --physcpubind=5 SendDemo -d 0 --smac a0:88:c2:0d:5e:28 --dmac 94:6d:ae:ac:f8:38 --sip 192.168.3.11 --dip 192.168.3.12 --sport 54277 --dport 54277 2>> ibv_info-1-stream1.log -N 320000