#include "main.h"
#include "usbd_cdc_if.h"

// Function Definitions
void Digest_Bytes(uint8_t* inputBuffer, uint32_t *bufferLength);
void Execute_Function(void);
void Reset_State(void);
void Func_Await_Command(void);
void Func_CRC_Echo(void);

// Define Function Codes
const uint8_t FUNC_AWAIT_COMMAND = 0;
const uint8_t FUNC_CRC_ECHO = 5;

// Function and Buffer States
uint8_t BUFFER[64];
uint8_t DIGEST_LIMIT = 1;
uint8_t CURRENT_FUNCTION = FUNC_AWAIT_COMMAND;
uint8_t FUNCTION_STATE = 0;
uint8_t INDEX = 0;

void Digest_Bytes(uint8_t* inputBuffer, uint32_t *bufferLength)
{
	//HAL_GPIO_WRITE_PIN(GPIOG, LED_PG1_Pin, GPIO_PIN_RESET);
	for (uint8_t i = 0; i < *bufferLength; i++) {
		BUFFER[INDEX] = inputBuffer[i];
		INDEX++;
		if (INDEX == DIGEST_LIMIT)
		{
			Execute_Function();
			INDEX = 0;
		}
	}
}

void Execute_Function(void)
{
	switch (CURRENT_FUNCTION)
	{
		case FUNC_AWAIT_COMMAND:
			Func_Await_Command();
			break;
		case FUNC_CRC_ECHO:
			Func_CRC_Echo();
			break;
	}
}

void Reset_State(void)
{
	DIGEST_LIMIT = 1;
	CURRENT_FUNCTION = FUNC_AWAIT_COMMAND;
	FUNCTION_STATE = 0;
}

void Func_Await_Command()
{
	CURRENT_FUNCTION = BUFFER[0];
	Execute_Function();
}

void Func_CRC_Echo()
{
	switch (FUNCTION_STATE)
	{
		case 0:
			DIGEST_LIMIT = 16; // Accept 16 bytes of random data
			FUNCTION_STATE = 1; // Increase the function state
			break;
		case 1:
		{
			// crc32 random data with "TAMU RoboMasters"
			uint8_t crc_buffer[32];
			uint8_t text[16] = "TAMU RoboMasters";
			for (uint8_t i = 0; i < 16; i++)
			{
				crc_buffer[i] = BUFFER[i];
			}
			// Add the characters
			for (uint8_t i = 16; i < 32; i++)
			{
				crc_buffer[i] = text[i];
			}
			
			// CRC32
			uint32_t polynomial = 0xEDB88320;
			uint32_t crc = 0xFFFFFFFF;
			for (uint8_t byteIndex = 0; byteIndex < 32; byteIndex++)
			{
				for (uint8_t i = 0; i < 8; i++)
				{
					uint8_t bit = (crc_buffer[byteIndex] & (1 << i)) >> i;
					if (((crc & 1) ^ bit) == 1)
					{
						crc = (crc >> 1) ^ polynomial;
					} else
					{
						crc = (crc >> 1);
					}
				}
			}
			crc = crc ^ 0xFFFFFFFF;
			
			// Split crc into 4 bytes
			uint8_t byte0 = (crc >> 24) & 0xFFFF;
			uint8_t byte1 = (crc >> 16) & 0xFFFF;
			uint8_t byte2 = (crc >> 8) & 0xFFFF;
			uint8_t byte3 = crc & 0xFFFF;
			
			uint8_t outBytes[] = {byte0, byte1, byte2, byte3};
			CDC_Transmit_FS(outBytes, 4);
			
			Reset_State();
			break;
		}
	}
}
