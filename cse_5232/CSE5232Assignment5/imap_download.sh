#!/bin/bash
cd $PWD/bin/ass5/
echo $PWD
java -classpath ./bin/:lib/gnuprologjava-0.2.6.jar ass5.Ass5 "$@"