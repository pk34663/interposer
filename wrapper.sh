#!/bin/bash
export LD_LIBRARY_PATH=/home/philk/src/libcli
LD_PRELOAD=./libpreload.so ./testwrite
