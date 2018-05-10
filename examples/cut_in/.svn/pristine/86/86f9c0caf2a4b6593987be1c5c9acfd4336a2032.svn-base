import socket 
import struct

def sendModelId(number,UDP_IP="127.0.0.1",UDP_PORT=25565):
  data=packData([number])
  sendData(data,UDP_IP,UDP_PORT)

def sendEmptyMessage(UDP_IP="127.0.0.1",UDP_PORT=25565):
  data=packData([0])
  sendData(data,UDP_IP,UDP_PORT)

def packData(data,data_type="i"):
  DATA = b""#byte string, that why the b is there. Other python versions might not need this
  #Converting all data into byte string and then concatenating
  for counter,letter in enumerate(data_type):
    DATA += struct.pack(letter,data[counter])
  
  return DATA

def sendData(DATA,UDP_IP="127.0.0.1",UDP_PORT=25565):
  sock = socket.socket(socket.AF_INET, # Internet
                      socket.SOCK_DGRAM) # UDP
  sock.sendto(DATA, (UDP_IP, UDP_PORT))

def receiveData(data_order="", UDP_IP = "127.0.0.1", UDP_PORT = 44445):   
  sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  sock.bind((UDP_IP, UDP_PORT))
  sock.settimeout(3)

  data_received=True
  message=()
  
  try:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    if data_order:
      message=struct.unpack(data_order, data)
  except socket.timeout:
    data_received=False

  return data_received, message
              
