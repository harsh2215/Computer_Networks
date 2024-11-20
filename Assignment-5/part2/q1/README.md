Leader Election for P2P network structure and logging files from all the nodes.

This program will run server and client functionalities on each node and after connecting all the nodes, Leader will be elected from the elect_leader function which then logs the files from each nodes to the Peer_files.log file.

File Structure: 

    Node1 --> 1_1.txt , 1_2.txt

    Node2 --> 2_1.txt

    Node3 --> 3_1.txt , 3_2.txt , 3_3.txt

    README.md

    q1.py

    Peer_files.log

Execution :

    python3 q1.py <node_id>

    run the above command with three different node_ids [1,2,3] on different terminals. after running this script for all the node_ids in the argument, leader election and file logging will happen.

    There is a sleep time of 20 seconds set for the file transfer after leader election. Hence, the program will wait for this time after leader election. If there are large number of files in the node directory make sure you change this time.

    this command will create node with the ID entered by the user from terminal.


Detailed explanation of the code execution and functions are given in the Report.