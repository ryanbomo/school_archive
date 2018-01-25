#!/bin/bash
cd ../
cd ./src/main
SCRIPT=$(readlink -f "$0")
./main -C -p 55443 -h "127.0.0.1" -U