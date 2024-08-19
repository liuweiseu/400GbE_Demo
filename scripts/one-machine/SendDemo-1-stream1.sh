#! /bin/bash

numactl --physcpubind=6 SendDemo -d 1 --smac 94:6d:ae:ac:f8:38 --dmac a0:88:c2:0d:5e:28 --sip 192.168.3.12 --dip 192.168.3.11 --sport 54277 --dport 54277 2>> ibv_info-1-stream0.log -N 320000