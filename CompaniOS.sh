#! /bin/sh

echo "Software Information"
system_profiler SPSoftwareDataType | awk 'NR>4'
echo ""
echo "Hardware Information"
system_profiler SPHardwareDataType | awk 'NR>4'

g++ main.cpp -std=c++17 -o CompaniOS
./CompaniOS