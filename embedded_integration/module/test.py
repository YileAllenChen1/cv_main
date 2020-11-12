import embedded_integration.util as util
from embedded_integration.command import String_Echo

ser = util.autoLocateSerial()
cmd = StringEcho('test text')
result = cmd.execute(ser)

print('Got: ' + result)