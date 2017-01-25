#!/bin/bash

# Create Huge Page
#------------------------------------------------------------------------------
sudo mkdir -p /mnt/huge 
sudo mount -t hugetlbfs nodev /mnt/huge
sudo sh -c "echo 256 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages"



