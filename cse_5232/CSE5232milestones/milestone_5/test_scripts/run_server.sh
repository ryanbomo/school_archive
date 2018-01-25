#!/bin/bash
cd ../
cd ./src/main
SCRIPT=$(readlink -f "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
SCRIPTPATH="$SCRIPTPATH/database.db"
./main -S -p $1 -d $SCRIPTPATH