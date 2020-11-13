import os
import zlib
from abc import ABCMeta, abstractmethod
from serial import Serial
from . import util

__all__ = ['CRC_Echo', 'String_Echo', 'Debug']

class Command(metaclass=ABCMeta):
    '''Abstract class used to create command classes.'''

    @abstractmethod
    def getCommandID(self) -> bytes:
        # Return a single byte containing the ID of the command.
        pass
    
    @abstractmethod
    def getCommandName(self) -> str:
        # Return a string containing the name of the command.
        pass

    @abstractmethod
    def execute(self, ser: Serial):
        # Execute the command.
        pass

    def __repr__(self):
        idValue = int.from_bytes(self.getCommandID(), 'big')
        nameValue = self.getCommandName()
        return f'{nameValue}(id={idValue})'

class CRC_Echo(Command):
    '''Returns True/False depending on whether
    the board respeonds correctly to the echo.
    '''

    def getCommandID(self):
        return b'\x05'  # ID: 5
    
    def getCommandName(self):
        return 'CRC_Echo'
    
    def execute(self, ser: Serial):
        random = os.urandom(16)
        text = b'TAMU RoboMasters'
        data = random + text
        myCRC = zlib.crc32(data)
        ser.write(self.getCommandID() + random)
        result = ser.read(4)
        resultCRC = int.from_bytes(result, 'big')

        return myCRC == resultCRC

class String_Echo(Command):
    '''Echos a string from the host the board.
    This will return the received string.
    '''

    def __init__(self, text):
        self.__text = text
    
    def getCommandID(self):
        return b'\x0A'  # ID: 10
    
    def getCommandName(self):
        return 'String_Echo'
    
    def execute(self, ser: Serial):
        text = self.__text.encode()
        length = len(text)
        ser.write(self.getCommandID())
        ser.write(len(text).to_bytes(4, 'big'))
        ser.write(text)
        outBytes = util.readDynamic(ser, length)
        outString = outBytes.decode()
        
        return outString
    
    def __repr__(self):
        idValue = int.from_bytes(self.getCommandID(), 'big')
        nameValue = self.getCommandName()
        return f'{nameValue}(id={idValue}, text="{self.__text}")'

class Debug(Command):
    '''Function to send custom data
    used for debugging.
    '''

    def getCommandID(self):
        return b'\x03'  # ID: 3
    
    def getCommandName(self):
        return 'Debug'
    
    def execute(self, ser: Serial):
        print('Trying to run debug')
        ser.write(self.getCommandID()) # Debug command code
        values = util.readDynamic(ser, 3)
        values = values.decode()
        print('Ran debug')
        print('Received the text: ' + values)