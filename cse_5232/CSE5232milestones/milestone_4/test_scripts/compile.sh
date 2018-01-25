#!/bin/bash
cd ../
cd ./src/main
gcc main.c server.c client.c asn_translate.h -o main -lsqlite3 -lpthread -libtasn1

