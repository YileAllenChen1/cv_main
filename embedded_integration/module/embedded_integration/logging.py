import serial
from . import settings

class LogSerial(serial.Serial):

    def write(self, data: bytes) -> int:
        if settings.RECORD_COMMUNICATION:
            pass

        if settings.PRINT_COMMUNICATION:
            print(f'Sent {len(data)} byte(s): {self.__bytesToHex(data)}')

        return super().write(data)
    
    def read(self, size=1) -> bytes:
        data = super().read(size)

        if settings.RECORD_COMMUNICATION:
            pass

        if settings.PRINT_COMMUNICATION:
            print(f'Read {len(data)} byte(s): {self.__bytesToHex(data)}')

        return data
    
    def __bytesToHex(self, data: bytes) -> str:
        out = ''
        for i in range(len(data)):
            out += f'{data[i]:02X}'
            if i != len(data)-1:
                out += '-'
        return out