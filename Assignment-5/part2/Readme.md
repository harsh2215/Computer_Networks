## Peer To Peer Network of 3 Nodes
It is a p2p network of 3 Nodes in which one node will act as a leader and other two will act as a peer. The leader will be selected by the nodes itself based on minimum value of Node Id. After that they communicates to each other via leader who contains the information of files log.

## Features
- **Node Initialization**: Each node is initialized with a unique ID, IP address, and port number. Nodes also have a list of peer nodes they can connect to.
- **Leader Election**: Nodes elect a leader based on the node ID. The node with the smallest ID becomes the leader.
- **File Sharing**: Nodes can request files from the leader. If the leader does not have the file, it will request the file from other nodes.
- **Log Maintenance**: The leader maintains a log of files available in the system and saves this log to a file.

## Steps to Reproduce

1. **Run the Script**: Execute the `server.py` script to start the nodes.
    ```bash
    python server.py
    ```
2. **File Request**: The code includes a request for a file named `12241300.png` from the leader.

3. You can change the filename to see different cases.

## Code Explanation
The flow of code execution is explained below:

1. **Node Initialization**: 
    - Each node is initialized with a unique ID, IP address, and port number.
    - Nodes also have a list of peer nodes they can connect to.
    - The `Node` class constructor sets up directories for storing files, initializes server and client threads, and sets up other necessary attributes.

2. **Starting the Server**:
    - The `start_server()` method starts a server socket that listens for incoming connections from other nodes.
    - It runs in a separate thread to allow concurrent handling of multiple connections.

3. **Connecting to Peers**:
    - The `connect_to_peer(peer_ip, peer_port)` method connects to peer nodes.
    - Each node attempts to connect to its peers and sends its node ID if a leader has not been elected yet.

4. **Handling Peer Connections**:
    - The `handle_peer_connection(conn)` method handles incoming connections from peers.
    - It processes different types of messages such as file requests, file transfers, and peer connections.

5. **Leader Election**:
    - The `elect_leader()` method elects a leader node based on the smallest node ID.
    - The leader node is responsible for maintaining the file log and coordinating file requests.

6. **Requesting Peer Logs**:
    - The `request_peer_logs()` method is called by the leader to request logs from all peers.
    - The leader collects these logs and stores them in a list.

7. **Saving Logs to File**:
    - The `save_log_to_file()` method saves the collected logs to a file named `peer_files.log` in the leader's directory.

8. **File Retrieval and Sharing**:
    - The `retrieve_file(file_path)` method retrieves a file from the leader's directory or requests it from another node if the leader does not have it.
    - The `send_file_chunks(file_name)` method sends file chunks to a requesting node.
    - The `request_file_from_leader(file_path)` method allows a non-leader node to request a file from the leader.

9. **Main Function**:
    - The `main()` function initializes the nodes, starts the server and client threads, elects the leader, requests peer logs, and handles file requests.
    - It also includes error handling for keyboard interrupts to gracefully shut down the nodes.










































