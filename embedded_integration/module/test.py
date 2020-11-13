import embedded_integration.util as util
from embedded_integration.command import String_Echo, Debug

from random import shuffle

ser = util.autoLocateSerial()

letters = [c for c in 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./,;']
for x in range(1000):
    shuffle(letters)

    text = ''
    for let in letters:
        text += let

    cmd = String_Echo(text)
    result = cmd.execute(ser)

    if result != text:
        print('ERROR!')
