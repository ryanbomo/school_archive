#!/bin/bash
cd ../
cd ./src/main
SCRIPT=$(readlink -f "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
SCRIPTPATH="$SCRIPTPATH/database.db"
./main -C -p $1 -h $2