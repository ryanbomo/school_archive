# CSE5232 Milestones

This is the README.md for my Network Programming class projects.  Each project is noted as a milestone.  Milestone 1 wasn't code related, and won't be tracked here.

## Milestone 2

The goal of Milestone 2 was to create an iterative gossip server capable of listening on both UDP and TCP ports.  The server would accept gossip from a user, check a database to see if the gossip had been told, and then broadcast new gossip.  Gossip was also to be stored in a SQLite database.

## Milestone 3

For Milestone 3 we need to update the iterative server from milestone 2 to be concurrent.  We also need to create either a command line or Android based client capable of sending gossip to our server.

## Milestone 4 (DOES NOT COMPILE)

ASN.1 Synchronization. We bit off a bit more than we could chew with this one. In trying to get ASN.1 sychronization working, we had a few milestones to achieve:

First, we need to convert the server from fork() to threads. Incorporating pthread isn't inherently difficult, but the added struct maintenance and synchronization issues need to be accounted for.

Second, we need to get ASN.1 incorporated in a way that meets the assignments requirements.

Third, we need our entire program to exist under one roof. Having separate client and server is not an option anymore.

We were able to accomplish the first and second points. There is an existing version of our program with pthread server that works correctly. There is also an existing cpp client that uses ASN.1 correctly.

Our main issue was converting from C++ to C in the client and getting it turned in on time. This did not happen. I will continue work on the program to get this finished, but this was turned in as an incomplete assignment.

## Milestone 5 (not started)

I have not looked into milestone 5 yet.





