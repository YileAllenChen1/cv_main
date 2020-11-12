import os
import zlib
import serial
from serial.serialutil import SerialException

# TEMPORARY FILE. THIS CODE WILL BE IMPLEMENTED ELSEWHERE.

def Func_Debug(ser):
    '''Function to send custom data
    used for debugging.
    '''
    print('Trying to run debug')
    ser.write(b'\x03') # Debug command code
    values = ser.read(3)
    values = values.decode()
    print('Ran debug')
    print('Received the text: ' + values)

def Func_String_Echo(ser, text: str):
    '''Echos a string from the host the board.
    This function will print the received string.
    '''
    text = text.encode()
    ser.write(b'\x0A')
    ser.write(len(text).to_bytes(4, 'big'))
    ser.write(text)
    outString = ''
    for i in range(len(text)):
        outString += ser.read(1).decode()
        print('Currently at: ' + outString)
    
    print(outString)