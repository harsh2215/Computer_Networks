import socket
import threading

# HOST = '192.168.11.39'  (IP of pravar's machine)
HOST = '127.0.0.1'
PORT = 8080 

def receive_updates(client_socket):
    while True:
        try:
            
            message = client_socket.recv(1024).decode()
            if not message:
                break
            print(message)
        
        except Exception as e:
            print("Error receiving updates:", e)
            break

def start_client():
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.connect((HOST, PORT))
        response = client_socket.recv(1024).decode()
        if response == "Server busy. Try again later.\n":
            print(response.strip())
            client_socket.close()
            return
        print("Connected to the server. Start typing to edit the document:")
        print(response)
        threading.Thread(target=receive_updates, args=(client_socket,)).start()
        
        while True:
            update = input()
            if update.lower() == "exit":
                break
            client_socket.sendall(update.encode())
    
    except Exception as e:
        print("Connection failed. Please check the server and try again.")
    finally:
        client_socket.close()

if __name__ == "__main__":
    start_client()
