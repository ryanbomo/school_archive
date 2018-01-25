#!/bin/bash
cd ../
cd ./src/main
gcc main.c -o main -L/usr/local/lib -lssl -lcrypto