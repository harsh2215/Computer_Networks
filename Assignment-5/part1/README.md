Collaborative Text Editor

This application provides a collaborative document editing system using socket programming. 

It allows maximum 10 clients at a time to connect at the server, share document updates and broadcast changes in real time. The server maintains a document with syncronization using threading. This doc is available to all the clients.

File Structure

    server.py - The code that manages the document, handles client connections, and broadcasts updates (server).
    client.py - The client-side code to connect to the server, edit the document, and receive updates.
    temp.txt - A file to store the document persistently on the server.

Execution :

1. run the server.py

Running this file will start server listening on port 8080. It will initialize the document variable from temp.txt (if file exists) else create the file.

2. run client.py

after successful connection client will start editing the document and send the updates to the server which server will broadcast to all the clients. and also it will be stored in the temp.txt.

--------------------------------------------------------------------------------------------------------------------------

To connect to a server on another machine, update HOST in client.py to the server's IP address.

In server.py, HOST is hardcoded with the wildcard address for IPv4 "0.0.0.0". This will allow any client to connect to the server via IPv4.
Remote clients can connect to the server via private IPv4 Ip if they are on the same network or via public ip if they are not on the same network. for this, client will have to change HOST address field in the client.py file.

--------------------------------------------------------------------------------------------------------------------------

Example Workflow

Server Output:

    python3 server.py 
    Server listening on port 8080
    New client connected: ('127.0.0.1', 40900)
    New client connected: ('127.0.0.1', 40902)
    Connection limit reached. Rejecting connection from ('127.0.0.1', 38960)
    Received update from client ('127.0.0.1', 40900): this is client1
    Broadcasting update to all clients


Client Output:

    python3 client.py 
    Connected to the server. Start typing to edit the document:
    hello
    world
    computer
    networks
    hey!
    let's complete assignment of socket programming.
    Hello harsh , pravar this side.
    hey pravar!
    let's do it tomorrow
    bye brother goodnight
    anyone's there who can work with me?
    yeah! But we can continue later since not all the clients are up now.
    I am also going...

    Document updated: this is client1
