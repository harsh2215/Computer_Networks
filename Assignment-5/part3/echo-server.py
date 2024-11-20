import socket
import threading

PORT = 8080

sockets = []

#Below function for creating socket
def server_socket(port):
    server_socket = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)  #line1
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1) #line2
    server_socket.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_V6ONLY, 0)#line3
    server_socket.bind(("", port))                                      #line4
    server_socket.listen(5)                                             #line5
    return server_socket                                                #line6


# This will handle connections from the client
def client_handle(client_socket, client_address):
    
    client_ip, client_port, _, _ = client_address   #extracting ip and port of the client to show on the terminal
        
    print(f"New Client connected : Client IP: {client_ip}  Client Port: {client_port}\n")
    while True:
        data = client_socket.recv(1024)
        if not data:
            break
        client_socket.sendall(data)  # Echo back the message
    print(f"Client with Client IP: {client_ip} and Client Port: {client_port} disconnected.\n")
    client_socket.close()

def listen_for_clients(server_socket):
    while True:
        
        try:
            client_socket, client_address = server_socket.accept()
            threading.Thread(target=client_handle, args=(client_socket, client_address)).start()
        except Exception as e:
            print(f"Error accepting connection: {e}")

if __name__ == "__main__":
    
    try:
        server_sock = server_socket(PORT)
        print(f"Server listening for connections on port {PORT}")
        
        listen_for_clients(server_sock)
    except Exception as e:
        print(f"Error binding socket: {e}")
