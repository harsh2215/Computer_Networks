import socket
import threading

HOST = '0.0.0.0'  
PORT = 8080

document_lock = threading.Lock()

clients = []
document = ""

try:
    with open("temp.txt", "r") as file:
        document = file.read()
except FileNotFoundError:
    open("temp.txt", "a").close()
except Exception as e:
    print("Error reading from temp.txt:", e)


def client_handle(client_socket, client_address):
    global document
    
    with document_lock:
        client_socket.sendall(document.encode())
    
    print(f"New client connected: {client_address}")
    
    while True:
        try:

            update = client_socket.recv(1024).decode()
            if not update:
                break  # client disconnected
            
            print(f"Received update from client {client_address}: {update}")
            
            with document_lock:
                document += update + "\n"
                with open("temp.txt", "a") as file:
                    file.write(update + "\n")
            
            broadcast_update(update, client_socket)
            print("Broadcasting update to all clients")
        
        except Exception as e:
            print(f"Error handling client {client_address}: {e}")
            break
    
    with document_lock:
        clients.remove(client_socket)
    print(f"Client {client_address} disconnected")
    client_socket.close()


def broadcast_update(update, sender_socket):
    with document_lock:
        for client in clients:
            if client != sender_socket:
                try:
                    client.sendall(f"Document updated: {update}".encode())
                except:
                    clients.remove(client)


def start_server():
    global clients
    try:
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.bind((HOST, PORT))
        server_socket.listen(10)
        print("Server listening on port 8080")
        
        while True:
            if len(clients) < 10:
                client_socket, client_address = server_socket.accept()
                clients.append(client_socket)
                threading.Thread(target=client_handle, args=(client_socket, client_address)).start()
            else:
                # Peek at the next client to log the address without accepting
                temp_socket, temp_address = server_socket.accept()
                print(f"Connection limit reached. Rejecting connection from {temp_address}")
                temp_socket.sendall(b"Server busy. Try again later.\n")
                temp_socket.close()


    
    except Exception as e:
        print("(Server starting error) Unable to start server on port 8080:", e)
    finally:
        server_socket.close()


if __name__ == "__main__":
    start_server()
