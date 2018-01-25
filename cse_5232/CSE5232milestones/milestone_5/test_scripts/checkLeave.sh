#!/bin/bash

&./serverTest.sh &
./peerTest.sh
trap "exit" INT TERM
trap "kill 0" EXIT