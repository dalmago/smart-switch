import socket
import sys
import time

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = ('192.168.2.11', 8080)
print('connecting to %s port %s' % server_address)

try:
    sock.connect(server_address)
    time.sleep(2)

    # # Send data
    message = b'toggle\r'
    print ('sending "%s"' % message)
    sock.sendall(message)

finally:
    print('closing socket')
    sock.close()
