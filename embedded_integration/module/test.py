from embedded_integration.integration_wrapper import IntegrationWrapper

iw = IntegrationWrapper()
text = iw.echoString('Hello there!')
print('Got: ' + text)