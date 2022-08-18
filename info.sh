#! /bin/bash
echo "=============================HostName=============================================="
hostname
echo "=============================Mount Related Info===================================="
pwd
ls
echo "=============================Name Space Information================================"
ls -al /proc/1/ns
echo "=============================Network IP Related Info==============================="
ip address
echo "=============================Cgroup Related Information============================"
cat /proc/1/cgroup
echo "=============================Process Information==================================="
ps aux