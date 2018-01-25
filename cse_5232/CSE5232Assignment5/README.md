# CSE5232 Assignment 5

This is the README.md for assinment 5 for CSE5232.  I am making an effort to put most of my coding assignments on Github this semester, to continue to building a showcase of my work.  Keep in mind that some of these assignments might be more slap-dash than others, and don't necessarily represent my best work.  I will maintain a more curated list of what  I am capable of.

## Goal

The goal of this is to implement an executable script that is capable of fetching emails from an IMAP server and storing them as txt in a desired folder structure.  The structure is defined in the assignment outline, and will be posted here when I have the chance.

## Assignment 

Here you have to write a program that communicates and parses IMAP commands (not
using an existing imap library).

Write a program (called from a script) that connects to a server provided 
in the first parameter over IMAP and ssl, e.g. over 
  - Java SSL sockets or,
  - with C/C++, using the openssl or gnutls libraries
  - with C/Java using a separate openssl/ncat process/script opened with exec/popen and/or named pipes 
and downloads all the emails on the server traversing the 
corresponding folders and storing emails in corresponding subfolders of the local folder 
creating subfolders with the same name as the IMAP folder. 
Each email will be stored in a file "content.txt", in a folder with the name given by 
a unique number on at least 5 digits (obtained by incrementing a counter) 
followed by a _ and further by the email address of the sender and after a
further _, by the title of the message (replacing all non-alphanumerical characters with "-").
Attachments will be stored in the same folder, if the email was multiparty.

The program should work on code01.fit.edu, downloading emails from gmail.com 
and should receive the openssl parameters, 
login name and and password from the command line or from the keyboard.
As an option, the program will be able to delete all downloaded mails from the server.

The program should be started with the command line:

$ imap_download.sh -S <server> -P <port> -l <login> [-p <password_if_not_stdin>] [-d] [-a] -f <folder>
    -d: delete after downloading
    -a: download from all folders
    -f: download messages from this folder

example:

$ imap_download.sh -S imap.gmail.com -P 993 -l msilaghi -p Passw0rd -d -f Mail/Garbeage -f Inbox -f Sent

Submit the assignment in a zip file  that also contains a "./compile.sh" script that 
compiles the assignment.


 HINT: In C you can start the openssl communication using popen in write mode and 
handle it via the corresponding pipe, while reading the results from a named pipe. 
Or you can exec it into a forked child with the file descriptors set. Submit compile and run scripts that take as parameter the server to contact. 
