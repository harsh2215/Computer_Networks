Echo Client/Server Application

This application creates echo-client and echo-server. when client connects to an echo-server the message which client sends to the server is reflected by the server and again get received at the client side. 

File structure:

    echo-server.py - This file opens a server for the clients to connect
    echo-client.py - This file asks client for the hostname or host IP to which it want to connect and connect to the echo-server on the port 8080.

execution:

1. run echo-server.py

executing this file will open echo-server to accept cconnections from client with bot hthe network interfaces IPv4 and IPv6.

2. run echo-client.py

running this file will ask the client for the hostname to connect and if it's able to resolve it then it will create a connection to the ehco-server on port 8080.



Detailed explanation of the workings of these programs are mentioned in the Report. Kindly refer Report for it.

