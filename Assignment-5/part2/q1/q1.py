import socket
import threading
import os
import time

nodes = {
    1: ("127.0.0.1", 8080),
    2: ("127.0.0.1", 8081),
    3: ("127.0.0.1", 8082)
}

log = "Peer_files.log"

def get_file_list(directory):
    files = []
    if os.path.exists(directory):
        # print("hello")
        files = os.listdir(directory)   #storing all the files from the dir to files variabal 
    return files





class PeerNode:
    
    
    def __init__(self, node_id):
        self.node_id = node_id
        self.server_address = nodes[node_id]
        self.peer_addresses = {k: v for k, v in nodes.items() if k != node_id}
        self.peer_connections = {}
        self.leader_id = None
        self.server_ready = threading.Event()

        threading.Thread(target=self.start_server, daemon=True).start()



    def start_server(self):
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)   #starting server fro IPv4 only
        server_socket.bind(self.server_address)
        server_socket.listen(5)
        print(f"Node {self.node_id} is listening on {self.server_address}...")
        self.server_ready.set()

        while True:
            connection, addr = server_socket.accept()
            threading.Thread(target=self.handle_connection, args=(connection,), daemon=True).start()


    def handle_connection(self, connection):
        try:
            data = connection.recv(1024).decode()
            # print("data" , data)
            
            if data.startswith("ID:"):
                peer_id = int(data.split(":")[1])
                self.peer_connections[peer_id] = connection
                print(f"Node {self.node_id} connected with Node {peer_id}.")
            elif data == "SEND":
                files = get_file_list(f"Node{self.node_id}")
                # print(files)
                connection.send("\n".join(files).encode())
            else:
                print(f"Node {self.node_id} received unknown command: {data}")
        except Exception as e:
            print(f"Error in connection handling: {e}")
    
    
    
    
    
    def connect_to_peers(self):
        for peer_id, address in self.peer_addresses.items():
            if peer_id not in self.peer_connections:
                while True:
                    try:
                        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                        sock.connect(address)
                        sock.send(f"ID:{self.node_id}".encode())
                        self.peer_connections[peer_id] = sock
                        print(f"Node {self.node_id} connected to Node {peer_id}.")
                        break
                    except ConnectionRefusedError:
                        time.sleep(2)


    def elect_leader(self):
        
        all_nodes = list(self.peer_connections.keys()) + [self.node_id]
        self.leader_id = min(all_nodes)
        print(f"Leader elected: Node {self.leader_id}")
        
        # code to disconnect non-leader nodse with each-other
        if self.node_id == self.leader_id:
            print(f"Leader Node {self.node_id} remains connected to all peers.")
        else:
            for peer_id, conn in self.peer_connections.items():
                if peer_id != self.leader_id:
                    conn.close()
                    print(f"Node {self.node_id} disconnected from Node {peer_id}.")


    def request_file_logs(self):
        if self.node_id == self.leader_id:
            with open(log, "w") as log_file:
                log_file.write(f"Leader Node {self.leader_id} files:\n")
                log_file.write("\n".join(get_file_list(f"Node{self.leader_id}")) + "\n\n")

                for peer_id, address in self.peer_addresses.items():
                    for _ in range(3):
                        try:
                            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
                                sock.connect(address)
                                sock.send("SEND".encode())
                                files = sock.recv(1024).decode()
                                log_file.write(f"Node {peer_id} files:\n{files}\n\n")
                                print(f"Files received from Node {peer_id}.")
                                break
                        except Exception as e:
                            print(f"Error receiving files from Node {peer_id}: {e}")
                            time.sleep(2)

    def run(self):
        self.server_ready.wait()
        time.sleep(3)
        self.connect_to_peers()
        time.sleep(3)
        self.elect_leader()
        time.sleep(3)
        self.request_file_logs()
        time.sleep(20)


if __name__ == "__main__":
    import sys

    if len(sys.argv) != 2:
        print("Usage: python3 q1.py <node_id>")
        sys.exit(1)

    node_id = int(sys.argv[1])
    if node_id not in nodes:
        print("Invalid node_id. Must be 1, 2, or 3.")
        sys.exit(1)

    node = PeerNode(node_id)
    node.run()
