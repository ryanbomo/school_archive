# Assignment 6 - OpenSSL in C

This is the README.md for assignment 6 for CSE 5232 - Network Programming.

## Requirements

Assignment a6 for Apr 17.
To submit in ascii text via the submit server (class cse4232 project a6).

Name:

ID:

Implement a client that uses openssl library in C to connect to a web server (to the certificate-based forum of the class), and to
download the index page of the forum, authenticating yourself using the certificate you got for the class.

https://debatedecide.fit.edu/proposals.php?organizationID=358&msg=&secure=on

## Structure

Currently works by taking in options and processing using getopt. Then it takes the URL give, processes it into an addr and context info.
This info is fed into the connector, which connects to the URL and uses the context information (or at least, it will). Then it checks certs,
and then closes.

## Scripts

  compile.sh - simple bash script to compile the program
  
  ```./compile.sh```
  
  run.sh - simple bash script to run the program
  
  ```./run.sh www.google.com```
  test1.sh - simple test script using address for assignment







