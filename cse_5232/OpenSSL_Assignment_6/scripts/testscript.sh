#!/bin/bash
cd ../
cd ./src/main
SCRIPT=$(readlink -f "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
SCRIPTPATH="$SCRIPTPATH/client.pem"
./main -s$1 -i$2 -c$SCRIPTPATH