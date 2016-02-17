#!/bin/bash

# Generate c-code test executable
echo
echo 'Making tests'
make

# Send via android debug bridge into /data/misc folder
echo
echo 'Transferring from Host to Device'
adb -d push test /data/misc

# Execute the c-code test on the device
echo
echo 'Running on Device shell'
adb -d shell /data/misc/test

# View modified /proc/(pid)/maps output for system_server's pid
echo
echo 'Testing modified filesystem'
adb -d shell cat /proc/$(adb -d shell ps | grep system_server | awk '{print $2}')/maps
