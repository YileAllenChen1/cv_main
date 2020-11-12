from abc import ABCMeta, abstractmethod
from serial import Serial

__all__ = ['CRC_Echo']

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

    def getCommandID(self):
        return b'\x05'
    
    def getCommandName(self):
        return 'CRC_Echo'
    
    def execute(self, ser: Serial):
        pass