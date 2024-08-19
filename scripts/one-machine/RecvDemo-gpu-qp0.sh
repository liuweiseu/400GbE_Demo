#! /bin/bash

# IP/mac/ibv-id :
# ens4np0 : 192.168.3.12 / 94:6d:ae:ac:f8:38 / 0
# ens4v0  : 192.168.4.12 / fa:45:6e:ec:98:45 / 1
# ens4v1  : 192.168.5.12 / de:9c:d2:ea:65:7b / 2

numactl --physcpubind=3 RecvDemo -d 1 --smac a0:88:c2:0d:5e:28 --dmac 94:6d:ae:ac:f8:38 --sip 192.168.3.11 --dip 192.168.3.12 --sport 54277 --dport 54277 --gpu 0 --nsge 1 2>> ibv_info-qp0.log