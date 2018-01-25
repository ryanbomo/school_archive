#!/bin/bash
cd ../
cd ./src/main
SCRIPT=$(readlink -f "$0")
& ./main -C -p 55443 -h "127.0.0.1" -U -m "PEER:JOHN:PORT=12345:IP=127.0.0.1%" &
sleep 2
& ./main -C -p 55443 -h "127.0.0.1" -U -m "PEERS?" &
sleep 7
./main -C -p 55443 -h "127.0.0.1" -U -m "PEERS?"
trap "exit" INT TERM
trap "kill 0" EXIT