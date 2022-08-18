import socket

TCP_IP = '0.0.0.0'
TCP_PORT = 49950

BUFFER_SIZE = 1

tcpServer = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
tcpServer.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
try:
    tcpServer.bind((TCP_IP, TCP_PORT))
except socket.error as e:
    print(str(e))

print("listening on PORT : ",TCP_PORT)
tcpServer.listen(5)


while True:
    (conn,(client_ip,client_port)) = tcpServer.accept()
    print("Connected with ",client_ip)
    with conn:
        while True:
            data = conn.recv(BUFFER_SIZE)
            if not data:
                break
            data = data.decode('utf-8')
            # #print(data)
            # counter = int(data)
            # i = 0
            # while i < 10:
            #     x = 0
            #     j = 0
            #     #print('data : ', counter)
            #     while j < counter:
            #     #print(i,x)
            #         x = x + j
            #         j = j + 1
            #     #print(x)
            #     i = i +1
            print(data)
            conn.sendall(bytes('1','utf-8'))
        print("Client with IP ", client_ip," has closed")
tcpServer.close()