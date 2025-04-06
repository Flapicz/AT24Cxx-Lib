#ifndef INC_AT24CXXX_H_
#define INC_AT24CXXX_H_

#ifdef TEST
#include "HAL_mock.h"

#else
#include "main.h"
#endif

/* Ustawienie pinów adresu pamięci. Obecność pinu w nazwie oznacza jego stan wysoki. */
typedef enum
{
	NO_PINS, A0, A1, A0A1, A2, A0A2, A1A2, A0A1A2
} addr_pins_t;

/* Modele AT24C */
typedef enum
{
	C01, C02, C04, C08, C16, C32, C64, C128, C256, C512, M01, M02
} eeprom_models_t;

typedef enum
{
	WP_DISABLE, WP_ENABLE
} wp_state_t;

struct eeprom
{
	I2C_HandleTypeDef *eeprom_handle;	 /* handler i2c otrzymany od aplikacji */
	uint8_t eeprom_device_adr; 		 	 /* adres urządzenia w przestrzeni i2c */
	uint8_t eeprom_adr_len; 		 	 /* rozmiar adresu pamięci w bajtach */
	eeprom_models_t eeprom_model;	 	 /* model urządzenia eeprom AT24Cxxx */

	GPIO_TypeDef *wp_gpio;			 	/* port gpio write protection; jeśli jest nullem, to wp niezainicjalizowane */
	uint16_t wp_gpio_pin;			 	/* pin gpio write protection */
};

/*function prototypes */
HAL_StatusTypeDef eeprom_initialize(struct eeprom *self, I2C_HandleTypeDef *hi2c, eeprom_models_t model, addr_pins_t addr_pins);
HAL_StatusTypeDef eeprom_initialize_wp(struct eeprom *self, GPIO_TypeDef *wp_gpio, uint16_t wp_gpio_pin);
HAL_StatusTypeDef eeprom_deinitialize_wp(struct eeprom *self);
HAL_StatusTypeDef eeprom_deinitialize(struct eeprom *self);
HAL_StatusTypeDef eeprom_write(struct eeprom *self, uint32_t memory_address, uint8_t *data, uint8_t data_size);
HAL_StatusTypeDef eeprom_read(struct eeprom *self, uint32_t memory_address, uint8_t *data, uint8_t data_size);
uint8_t eeprom_test(uint8_t value);


#endif /* INC_AT24CXXX_H_ */
