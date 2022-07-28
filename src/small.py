import socket

ip = "192.168.1.112"     # этот адрес 
udpPort   = 54545;


sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)



sock.bind(  (ip,udpPort)   )



sock.sendto('Get', (ip, udpPort))

conn,adr = sock.recvfrom(128,udpPort)
