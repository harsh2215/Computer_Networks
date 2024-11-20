import socket

def get_server_details():
    address = input("Enter the server address : ").strip()
    return address or "localhost", 8080

def resolve_address(host, port):
    
    results = socket.getaddrinfo(host, port, socket.AF_UNSPEC, socket.SOCK_STREAM)  
    
    for res in results:
        family, sock_type, proto, _, sockaddr = res
        if family in [socket.AF_INET, socket.AF_INET6]:
            return family, sock_type, proto, sockaddr
    return None

if __name__ == "__main__":
    host, port = get_server_details()
    address_info = resolve_address(host, port)
    
    flag = 1
    
    if not address_info:
        print("Unable to resolve the address.")
        flag = 0
    
    if (flag):
        family, sock_type, proto, sockaddr = address_info
        
        with socket.socket(family, sock_type, proto) as client_socket:
            client_socket.connect(sockaddr)
            print(f"Successfully connected to the server at {sockaddr}\n")

            while True:
                message = input("Enter message ('q' to quit): ")
                if message.lower() == "q":
                    break
                client_socket.sendall(message.encode())
                response = client_socket.recv(1024).decode()
                print(f"Echoed message from the server: {response}\n")
