#include "HAL_mock.h"
#include "AT24Cxxx.h"

#define HAL_MAX_DELAY 10

uint8_t reg[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void HAL_Delay(uint8_t time){}

void HAL_GPIO_WritePin(GPIO_TypeDef *gpio_Port, uint16_t gpio_pin, GPIO_PinTypeDef value)
{
	if(value == GPIO_PIN_SET) gpio_Port->state = 1;
	else if(value == GPIO_PIN_RESET) gpio_Port->state = 0;
}

HAL_StatusTypeDef HAL_I2C_IsDeviceReady(I2C_HandleTypeDef *handle, uint8_t address, uint8_t size, uint8_t time)
{
	return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *handle, uint8_t i2c_address, uint32_t address, uint8_t size, uint8_t *data, uint8_t data_size, uint8_t time)
{
	handle->address = i2c_address;
	handle->memory_address = address;
	for(uint8_t i=0; i<data_size; i++)
	{
		reg[i] = *(data+i);
	}
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *handle, uint8_t i2c_address, uint32_t address, uint8_t size, uint8_t *data, uint8_t data_size, uint8_t time)
{
	for(uint8_t i=0; i<data_size; i++)
	{
		*(data+i) = reg[i];
	}
	return HAL_OK;
}