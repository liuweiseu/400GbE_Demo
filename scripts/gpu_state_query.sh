##########################################################################
# File Name: gpu_state_query.sh
# Author: Wei Liu
# mail: liuwei_berkeley@berkeley.edu
# Created Time: Fri 03 Nov 2023 02:55:16 PM UTC
#########################################################################
#!/bin/bash
echo timestamp pstate power pcie_link_gen pcie_link_width temperature util 
nvidia-smi --query-gpu=timestamp,pstate,power.draw,pcie.link.gen.current,pcie.link.width.current,temperature.gpu,utilization.gpu --format=csv -lms 1000 -i $1     
#nvidia-smi --query-gpu=timestamp,pstate,power.draw,pcie.link.gen.current,pcie.link.width.current,temperature.gpu,utilization.gpu,clocks.current.graphics,clocks.current.memory,clocks.current.sm --format=csv -lms 1000      
