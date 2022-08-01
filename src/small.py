import socket
import time

orgTimeOut = socket.getdefaulttimeout();
socket.setdefaulttimeout(1)

ip = "192.168.1.112"     # этот адрес 
remouteUdpPort   = 54545;
buf = bytearray(128)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("",54546))

while True:
    buf='IsSomebodyHere'.encode('UTF-8')
    sock.sendto( buf, (ip, remouteUdpPort))
    try:
        conn,adr = sock.recvfrom(128 )
        print(conn.decode('UTF-8'))
    except:
        print('sendto:timeout')
        exit(0)
    buf='Get distance'.encode('UTF-8')        
    for i in range(1,10):
        sock.sendto( buf,(ip, remouteUdpPort) )
        conn,adr = sock.recvfrom(128 )
        s = conn.decode('UTF-8')
        print(s)
        time.sleep(2.1)

    