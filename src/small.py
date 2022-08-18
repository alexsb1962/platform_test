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
        content,adr = sock.recvfrom(128 )
        print(content.decode('UTF-8'))
    except:
        print('IsSomebodyHere: timeout')
        exit(0)
    else:
                

    buf='Get distance'.encode('UTF-8')        
    for i in range(1,10):
        sock.sendto( buf,(ip, remouteUdpPort) )
        try:
            content,adr = sock.recvfrom(128 )
        except:
            print('Get Distance : timeout')
        else:
            print(content.decode('UTF-8'))

        time.sleep(1.0)

    