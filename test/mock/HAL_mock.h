#ifndef HAL_H
#define HAL_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef struct __I2C_HandleTypeDef
{
	uint8_t Id;
	uint8_t address;
	uint32_t memory_address;
} I2C_HandleTypeDef;

typedef struct __GPIO_TypeDef
{
	uint8_t Id;
	uint8_t state;
} GPIO_TypeDef;

typedef enum
{
	GPIO_PIN_SET,
	GPIO_PIN_RESET
} GPIO_PinTypeDef;

typedef enum
{
	HAL_OK,
	HAL_ERROR
} HAL_StatusTypeDef;

void HAL_Delay(uint8_t time);
void HAL_GPIO_WritePin(GPIO_TypeDef *gpio_Port, uint16_t gpio_pin, GPIO_PinTypeDef value);
HAL_StatusTypeDef HAL_I2C_IsDeviceReady(I2C_HandleTypeDef *handle, uint8_t address, uint8_t size, uint8_t time);
HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *handle, uint8_t i2c_address, uint32_t address, uint8_t size, uint8_t *data, uint8_t data_size, uint8_t time);
HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *handle, uint8_t i2c_address, uint32_t address, uint8_t size, uint8_t *data, uint8_t data_size, uint8_t time);

#endif