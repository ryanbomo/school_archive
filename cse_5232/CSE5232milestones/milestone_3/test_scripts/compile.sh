#!/bin/bash
cd ../
cd ./src/main
g++  client.cpp -o client
gcc main.c -o main -lsqlite3

