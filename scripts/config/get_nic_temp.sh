##########################################################################
# File Name: get_nic_temp.sh
# Author: Wei Liu
# mail: liuwei_berkeley@berkeley.edu
# Created Time: Sat 18 Nov 2023 02:52:28 AM UTC
#########################################################################
#!/bin/bash
watch -n 1 mget_temp -d $1
