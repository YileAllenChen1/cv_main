from . import util
from . import command

class IntegrationWrapper():
    '''This is wrapper for Serial that adds functionality
    through the commands defined in command.py
    '''

    def __init__(self, port='auto'):
        if port == 'auto':
            self.__serial = util.autoLocateSerial()
        else:
            self.__serial = util.openFreshSerial(port)
    
    def write(self, data: bytes):
        '''Write raw bytes to the serial port.
        '''
        self.__serial.write(data)
    
    def read(self, length: int) -> bytes:
        '''Read raw bytes from the serial port.
        '''
        return self.__serial.read(length)
    
    def echoString(self, text: str) -> str:
        '''Bounce a string to and from the board.

        This returns the text received from the board.
        '''
        cmd = command.String_Echo(text)
        return cmd.execute(self.__serial)