import os
from serial.serialutil import SerialException
from . import command
from . import settings
from .logging import LogSerial as Serial

__all__ = ['autoLocateSerial', 'openFreshSerial', 'readDynamic']

def autoLocateSerial() -> Serial:
    platform = os.sys.platform
    if platform.startswith('linux'):
        ser = __locateLinuxSerial()
    elif platform.startswith('win'):
        ser = __locateWindowsSerial()
    else:
        raise NotImplementedError(f'Platform: {platform} is unsupported.')
    return ser

def openFreshSerial(port: str) -> Serial:
    return Serial(port, timeout=settings.SERIAL_TIMEOUT)

def readDynamic(ser: Serial, length: int) -> bytes:
    data = ser.read(length + 4)
    return data[4:]

def __locateLinuxSerial() -> Serial:
    print('Locating serial device on Linux host.')
    devices = os.listdir('/dev')
    serialDevices = filter(lambda x: x.startswith('tty'), devices)
    located = False
    for serialDevice in serialDevices:
        if serialDevice == 'tty':
            continue
        print('Checking device: /dev/' + serialDevice)
        try:
            ser = Serial('/dev/' + serialDevice, timeout=0.1)
            cmd = command.CRC_Echo()
            if cmd.execute(ser):
                print('Device: /dev/' + serialDevice + ' responded correctly')
                located = True
                break
            else:
                print('Invalid response from /dev/' + serialDevice)
        except SerialException:
            continue
    if located:
        ser.close()
        return openFreshSerial('/dev/' + serialDevice)
    else:
        raise IOError('Unable to locate serial port. Possible permissions issue?')

def __locateWindowsSerial() -> Serial:
    print('Locating serial device on Windows host.')
    serialDevices = ['COM' + str(x) for x in range(10)]
    located = False
    for serialDevice in serialDevices:
        print('Checking device: '+ serialDevice)
        try:
            ser = Serial(serialDevice, timeout=0.1)
            cmd = command.CRC_Echo()
            if cmd.execute(ser):
                print('Device: ' + serialDevice + ' responded correctly')
                located = True
                break
            else:
                print('Invalid response from ' + serialDevice)
        except SerialException:
            continue
    if located:
        ser.close()
        return openFreshSerial(serialDevice)
    else:
        raise IOError('Unable to locate serial port.')