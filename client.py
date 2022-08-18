import socket

SERVER_IP = input("Enter The Server IP Address : ")
SERVER_PORT = 49950
client_conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
    client_conn.connect((SERVER_IP, SERVER_PORT))
except socket.error as e:
    print(e)
    exit()

print("Connection Established with ",SERVER_IP," WITH PORT : ", SERVER_PORT)
while True:
    data = input("Enter 1 byte data to send : ")
    if data == "0" :
        client_conn.close()
        break
    client_conn.sendall(bytes(data,'utf=8'))
    recv_data = client_conn.recv(1).decode('utf-8')
    print("Received Data From Server : ",recv_data)

print("Closing The clinet")

