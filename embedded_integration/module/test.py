import embedded_integration.util as util
from embedded_integration.command import String_Echo, Debug
from embedded_integration.integration_wrapper import IntegrationWrapper

from random import shuffle

iw = IntegrationWrapper()
text = iw.echoString('Hello there!')
print('Got: ' + text)