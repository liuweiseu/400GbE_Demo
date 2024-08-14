#! /bin/bash

SendDemo -d 0 --streams 2 --smac a0:88:c2:0d:5e:28,a0:88:c2:0d:5e:28 --dmac 94:6d:ae:ac:f8:38,94:6d:ae:ac:f8:38 --sip 192.168.3.2,192.168.3.2 --dip 192.168.3.12,192.168.3.12 --sport 54277,54278 --dport 54277,54278 2>> ibv_info-2-streams.log -N 320000