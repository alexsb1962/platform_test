import socket

orgTimeOut = socket.getdefaulttimeout();
socket.setdefaulttimeout(1)

ip = "192.168.1.112"     # этот адрес 
remouteUdpPort   = 54545;
buf = bytearray(128)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#sock.bind((ip,54546))

while True:
    buf='Restart'.encode('UTF-8')
    sock.sendto( buf, (ip, remouteUdpPort))
    try:
        conn,adr = sock.recvfrom(128 )
        print(conn)
    except:
        print('timeout')