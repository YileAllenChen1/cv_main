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