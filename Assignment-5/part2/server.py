import socket
import threading
import os
import time

# Ques 1 and 2 both are implemented in this code

class Node:
    def __init__(self, node_id, ip, port, peer_ips_ports):
        os.makedirs(f"files{node_id+1}", exist_ok=True)
        self.node_id = node_id
        self.ip = ip
        self.port = port
        self.peers = peer_ips_ports
        self.leader_id = None
        self.file_log = []
        self.peer_details = []
        self.is_running = True
        self.files_dir = f"./files{node_id+1}"

        # Initialize server and client threads
        self.server_thread = threading.Thread(target=self.start_server)        
        self.client_threads = [threading.Thread(target=self.connect_to_peer, args=(peer_ip, peer_port)) for peer_ip, peer_port in self.peers]

    def start_server(self):
        '''Start server to listen for incoming connections'''
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
            server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            server_socket.bind((self.ip, self.port))
            server_socket.listen()
            print(f"Node {self.node_id} listening on {self.ip}:{self.port}")
            server_socket.settimeout(1)
            while self.is_running:
                try:
                    conn, addr = server_socket.accept()
                    thread = threading.Thread(target=self.handle_peer_connection, args=(conn,))
                    thread.start()
                except socket.timeout:
                    continue

    def connect_to_peer(self, peer_ip, peer_port):
        '''Connect to peer nodes'''
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
            client_socket.connect((peer_ip, peer_port))
            if not self.leader_id:
                client_socket.sendall(str(self.node_id).encode('utf-8'))            
            client_socket.close()

    def handle_peer_connection(self, conn):
        '''Handle incoming connections from peers'''
        try:
            data = conn.recv(1024).decode('utf-8')
            if data == "SEND":
                file_list = self.get_file_list()
                conn.sendall(file_list.encode('utf-8'))
            elif data.startswith("GET"):
                print(f"Leader Node {self.node_id} received request for file {data.split(':')[1]}")
                file_name = data.split(":")[1]
                file_data = self.retrieve_file(file_name)
                if isinstance(file_data, bytes):
                    conn.sendall("START_FILE".encode('utf-8'))
                    chunk_size = 1024
                    for i in range(0, len(file_data), chunk_size):
                        conn.sendall(file_data[i:i+chunk_size])
                else:
                    conn.sendall(file_data.encode('utf-8'))
            elif data.startswith("SENDFILE"):
                print(f"Node {self.node_id} received request for file {data.split(':')[1]} from leader")
                file_name = data.split(":")[1]
                file_chunks = self.send_file_chunks(file_name)
                if file_chunks:
                    conn.sendall("START_FILE".encode('utf-8'))
                    for chunk in file_chunks:
                        conn.sendall(chunk)
                    print(f"Node {self.node_id} finished sending file {file_name}")
                else:
                    conn.sendall('File not found'.encode('utf-8'))
            else:
                peer_id = int(data)
                port = next(port for ip, port in self.peers if ip == conn.getpeername()[0])
                self.peer_details.append((peer_id, (conn.getpeername()[0], port)))
                print(f"Node {self.node_id} connected to peer {peer_id}")
        except Exception as e:
            print(f"Error in handling clients: {e}")

    def elect_leader(self):
        '''Elect leader node based on node_id'''
        all_ids = [self.node_id] + [peer_id for peer_id, _ in self.peer_details]
        self.leader_id = min(all_ids)
        print(f"Node {self.node_id} elected leader: {self.leader_id}")

    def request_peer_logs(self):
        '''Request logs from all peers'''
        if self.node_id == self.leader_id:
            for peer_ip, peer_port in self.peers:
                try:
                    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                        s.connect((peer_ip, peer_port))
                        s.sendall("SEND".encode('utf-8'))
                        log_data = s.recv(1024).decode('utf-8')
                        logs = f"{peer_ip}:{peer_port}-{log_data}"
                        self.file_log.append(logs)
                        print(f"Leader {self.node_id} received log from {peer_ip}:{peer_port}")
                except:
                    print(f"Could not connect to peer at {peer_ip}:{peer_port}")

    def save_log_to_file(self):
        '''Save logs to file'''
        if self.node_id == self.leader_id:
            with open(f"{self.files_dir}/peer_files.log", "w") as f:
                print(f"Leader received logs:\n{self.file_log}\n")
                for log in self.file_log:
                    f.write(log + "\n")
            print("Log saved to peer_files.log")

    def get_file_list(self):
        '''Return list of files in directory'''
        files = []
        for file_name in os.listdir(self.files_dir):
            full_path = os.path.join(self.files_dir, file_name)
            if os.path.isfile(full_path):
                files.append(full_path)
        return ",".join(files)

    def retrieve_file(self, file_path):
        '''Retrieve file from leader directory or ask another node for file'''
        try:
            file_exists = False
            for file in os.listdir(self.files_dir):
                if file == file_path:
                    print(f"Leader has file {file_path}")
                    with open(f"{self.files_dir}/{file_path}", "rb") as f:
                        return f.read()

            with open(f"{self.files_dir}/peer_files.log", "r") as f:
                file_data = f.read().split("\n")
                for data in file_data:
                    if data.find(file_path) != -1:
                        print(f"Node {self.node_id} found file {file_path} in log")
                        ip, port = data.split("-")[0].split(":")
                        print(f"Node {self.node_id} asking node {ip}:{port} for file {file_path}")
                        file_exists = True
                        break

            if file_exists:
                with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                    s.connect((ip, int(port)))
                    s.sendall(f"SENDFILE:{file_path}".encode('utf-8'))
                    message = s.recv(1024).decode('utf-8')
                    if message.startswith("START_FILE"):
                        chunks = b""
                        while True:
                            chunk = s.recv(1024)
                            if len(chunk) < 1024:
                                chunks += chunk
                                break
                            chunks += chunk
                        return chunks
                    else:
                        print(f"Node {self.node_id} received error: {message}")
                        return "404 Not Found"
            else:
                return "404 Not Found"
        except Exception as e:
            print(f"Error: {e}")
            return "500 Internal Server Error"

    def send_file_chunks(self, file_name):
        '''Send file chunks to requesting node'''
        for file in os.listdir(self.files_dir):
            full_path = os.path.join(self.files_dir, file)
            if os.path.isfile(full_path):
                if file == file_name:
                    def file_chunks():
                        with open(full_path, "rb") as f:
                            while chunk := f.read(1024):
                                yield chunk
                    return file_chunks()
        return None

    def request_file_from_leader(self, file_path):
        '''Request file from leader node'''
        try:
            if self.node_id != self.leader_id:
                print(f"Node {self.node_id} asking leader {self.leader_id} for file {file_path}")

                ip, port = self.get_leader_ip_port()
                if not ip and not port:
                    print("Leader IP and port not found")
                    return
                with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                    s.connect((ip, port))
                    s.sendall(f"GET:{file_path}".encode('utf-8'))
                    message = s.recv(1024).decode('utf-8')
                    if message.startswith("START_FILE"):
                        with open(f"{self.files_dir}/{file_path}", "wb") as f:
                            while True:
                                chunk = s.recv(1024)
                                if len(chunk) < 1024:
                                    f.write(chunk)
                                    break
                                f.write(chunk)
                        print(f"Node {self.node_id} received file {file_path} from leader")
                    else:
                        print(f"Node {self.node_id} received error: {message}")
        except Exception as e:
            print(f"Error in Requesting: {e}")

    def get_leader_ip_port(self):
        '''Return IP and port of leader node'''
        for peer_id, (ip, port) in self.peer_details:
            if peer_id == self.leader_id:
                return ip, port
        return None, None

def main():
    try:
        node_ips_ports = [
            ("127.0.0.1", 8001),
            ("127.0.0.1", 8002),
            ("127.0.0.1", 8003)
        ]
        nodes = []
        for i, (ip, port) in enumerate(node_ips_ports):
            peer_ips_ports = node_ips_ports[:i] + node_ips_ports[i+1:]
            node = Node(i, ip, port, peer_ips_ports)
            nodes.append(node)

        for node in nodes:
            node.server_thread.start()

        time.sleep(1)

        for node in nodes:
            for client_thread in node.client_threads:
                client_thread.start()

        time.sleep(1)

        for node in nodes:
            node.elect_leader()

        time.sleep(1)

        for node in nodes:
            if node.node_id == node.leader_id:
                node.request_peer_logs()
                node.save_log_to_file()

        time.sleep(1)
        # node 2 is request file from leader node
        nodes[2].request_file_from_leader(f"12241170.jpg")

    except KeyboardInterrupt:
        print("Caught keyboard interrupt, exiting")
        for node in nodes:
            node.is_running = False
        for node in nodes:
            node.server_thread.join()
            for client_thread in node.client_threads:
                client_thread.join()        
    finally:
        for node in nodes:
            node.is_running = False
        for node in nodes:
            node.server_thread.join()
            for client_thread in node.client_threads:
                client_thread.join()
        print("Exiting program")

if __name__ == "__main__":
    main()
