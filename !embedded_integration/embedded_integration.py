import os
import zlib
import serial
from serial.serialutil import SerialException

def Func_CRC_Echo(ser):
    '''Returns True/False depending on whether
    the board respeonds correctly to the echo.
    '''
    random = os.urandom(16)
    text = b'TAMU RoboMasters'
    data = random + text
    myCRC = zlib.crc32(data)
    ser.write(b'\x05' + random)
    result = ser.read(4)
    resultCRC = int.from_bytes(result, 'big')

    #print('Host CRC:  ' + hex(myCRC).upper())
    #print('Board CRC: ' + hex(resultCRC).upper())

    return myCRC == resultCRC

def Func_String_Echo(ser, text: str):
    '''Echos a string from the host the board.
    This function will print the received string.
    '''
    text = text.encode()
    ser.write(b'\x0A')
    ser.write(len(text).to_bytes(4, 'big'))
    ser.write(text)
    receivedBytes = ser.read(len(text))
    receivedText = receivedBytes.decode()
    print(receivedText)

def locateLinuxSerial():
    print('Locating serial device on Linux host.')
    devices = os.listdir('/dev')
    serialDevices = filter(lambda x: x.startswith('tty'), devices)
    located = False
    for serialDevice in serialDevices:
        print('Checking device: /dev/' + serialDevice)
        try:
            ser = serial.Serial('/dev/' + serialDevice, timeout=1)
            if Func_CRC_Echo(ser):
                print('Device: /dev/' + serialDevice + ' responded correctly')
                located = True
                break
            else:
                print('Invalid response from /dev/' + serialDevice)
        except SerialException:
            continue
    if located:
        return ser
    else:
        raise Exception('Unable to locate serial port.')

def locateWindowsSerial():
    print('Locating serial device on Windows host.')
    serialDevices = ['COM' + str(x) for x in range(10)]
    located = False
    for serialDevice in serialDevices:
        print('Checking device: '+ serialDevice)
        try:
            ser = serial.Serial(serialDevice, timeout=1)
            if Func_CRC_Echo(ser):
                print('Device: ' + serialDevice + ' responded correctly')
                located = True
                break
            else:
                print('Invalid response from ' + serialDevice)
        except SerialException:
            continue
    if located:
        return ser
    else:
        raise Exception('Unable to locate serial port.')

def main():
    platform = os.sys.platform
    if platform.startswith('linux'):
        ser = locateLinuxSerial()
    elif platform.startswith('win'):
        ser = locateWindowsSerial()
    else:
        raise Exception('Unsupported platform.')
    Func_String_Echo(ser, 'Hello there!')
    print('Sent')

if __name__ == '__main__':
    main()
