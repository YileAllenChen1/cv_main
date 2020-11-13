from embedded_integration import IntegrationWrapper

iw = IntegrationWrapper()
text = iw.echoString('Hello there!')
print('Got: ' + text)