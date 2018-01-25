#!/bin/bash
cd ../
cd ./src/main
SCRIPT=$(readlink -f "$0")
./main -s$1