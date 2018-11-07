import socket               
 
sock = socket.socket()
 
host = "192.168.178.169" #ESP32 IP in local network
port = 80             #ESP32 Server Port    
data = '';
if sock.connect((host, port)):
	print ('connect successfully')
data = sock.recv(1024)
print(data)
 
sock.close()