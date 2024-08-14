#! /bin/bash

watch -n 1 "nvidia-smi -q  | grep Throughput"
