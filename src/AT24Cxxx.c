#include "AT24Cxxx.h"

#ifdef TEST
#define HAL_MAX_DELAY 10
#endif

#define EEPROM_BASE_ADR 0xA0
#define GPIO_WRITE_DELAY 5

/** @brief Ustawia pin gpio Write Protection, jeśli taki istnieje
  * @param self: Wskażnik na strukturę pamięci EEPROM
  * @param wp_state: Stan, na jaki ma być ustawiony pin gpio. Uruchumienie write_protection oznacza że żaden wpis się nie wykona
  * @retval None
  */
static void wp_set (struct eeprom *self, wp_state_t wp_state)
{
	if (self->wp_gpio != NULL)
	{
		if (wp_state) HAL_GPIO_WritePin(self->wp_gpio, self->wp_gpio_pin, GPIO_PIN_SET);
		else HAL_GPIO_WritePin(self->wp_gpio, self->wp_gpio_pin, GPIO_PIN_RESET);
		HAL_Delay(GPIO_WRITE_DELAY);
	}
}


/** @brief Przenosi bity adresu pamięci do adresu urządzenia. W zależności od modelu będzie to od 0 do 3 bitów
  * @param self: Wskażnik na strukturę pamięci EEPROM
  * @param memory_address: Adres pamięci
  * @retval Uzupełniony adres urządzenia EEPROM
  */
static uint8_t fill_device_adr(struct eeprom *self, uint32_t memory_address)
{
	/*dla wielu modeli adres urządzenia zależy od 1/2/3 bitów adresu wpisu/odczytu!
	dla M01, C04 - jeden bit
	dla M02, C08 - dwa bity
	dla C16 - trzy bity */

	uint8_t eeprom_temp_device_adr = self->eeprom_device_adr;
	if(self->eeprom_model == M01 || self->eeprom_model == C04)
		{
			eeprom_temp_device_adr &= 0xFD; //wyzerowanie bitu 1 eeprom_addr (na wszelki wypadek)
			if(self->eeprom_model == M01) eeprom_temp_device_adr = eeprom_temp_device_adr | ((memory_address >> 15) & 0x2); //przesunięcie bitu 16 adresu pamięci na bit 1, wyzerowanie pozostałych bitów, wstawienie tego bitu do adresu
			else eeprom_temp_device_adr = eeprom_temp_device_adr | ((memory_address >> 7) & 0x2);
		}

	else if(self->eeprom_model == M02 || self->eeprom_model == C08)
		{
			eeprom_temp_device_adr &= 0xF9;
			if(self->eeprom_model == M02) eeprom_temp_device_adr = eeprom_temp_device_adr | ((memory_address >> 15) & 0x6);
			else eeprom_temp_device_adr = eeprom_temp_device_adr | ((memory_address >> 7) & 0x6);
		}

	else if(self->eeprom_model == C16)
		{
			eeprom_temp_device_adr &= 0xF1;
			eeprom_temp_device_adr = eeprom_temp_device_adr | ((memory_address >> 7) & 0xE);
		}
	return eeprom_temp_device_adr;
}

/** @brief Inicjalizuje EEPROM
  * @param self: Wskażnik na strukturę pamięci EEPROM
  * @param hi2c: Handler I2C
  * @param model: Model pamięci EEPROM
  * @param addr_pins: Ustawienie pinów adresowych pamięci EEPROM
  * @retval HAL_OK lub HAL_ERROR
  */
HAL_StatusTypeDef eeprom_initialize(struct eeprom *self, I2C_HandleTypeDef *hi2c, eeprom_models_t model, addr_pins_t addr_pins)
{
	memset(self, 0, sizeof(*self));

	self->eeprom_handle = hi2c;
	self->eeprom_device_adr = EEPROM_BASE_ADR | (addr_pins << 1);
	self->eeprom_model = model;

	if (model <= 4) self->eeprom_adr_len = 1;
	else if (model <= 11) self->eeprom_adr_len = 2; //model: 0 - C01, 11 - M02
	else return HAL_ERROR;

	return HAL_I2C_IsDeviceReady(self->eeprom_handle, self->eeprom_device_adr, 1, HAL_MAX_DELAY);
}

/** @brief Deinicjalizuje EEPROM
  * @param self: Wskażnik na strukturę pamięci EEPROM
  * @retval HAL_OK lub HAL_ERROR
  */
HAL_StatusTypeDef eeprom_deinitialize(struct eeprom *self)
{
	return eeprom_deinitialize_wp(self);
}

/** @brief Inicjalizuje Write Protection poprzez podanie informacji o używanym pinie GPIO
  * @param self: Wskażnik na strukturę pamięci EEPROM
  * @param gpio_port: Wskaźnik na strukturę portu GPIO, która ma być używana jako Write Protection
  * @param gpio_pin: Numer pinu w porcie GPIO
  * @retval HAL_OK lub HAL_ERROR
  */
HAL_StatusTypeDef eeprom_initialize_wp(struct eeprom *self, GPIO_TypeDef *gpio_port, uint16_t gpio_pin)
{
	self->wp_gpio = gpio_port;
	self->wp_gpio_pin = gpio_pin;
	wp_set(self, WP_ENABLE);
	return HAL_OK;
}

/** @brief Deinicjalizuje Write Protection
  * @param self: Wskażnik na strukturę pamięci EEPROM
  * @retval HAL_OK lub HAL_ERROR
  */
HAL_StatusTypeDef eeprom_deinitialize_wp(struct eeprom *self)
{
	wp_set(self, WP_DISABLE);
	self->wp_gpio = NULL;
	self->wp_gpio_pin = 0;
	return HAL_OK;
}

/** @brief Zapis danych do pamięci EEPROM
  * @param self: Wskażnik na strukturę pamięci EEPROM
  * @param memory_address: Adres miejsca zapisu słowa
  * @param data: Wskaźnik na dane do zapisu
  * @param data_size: Ilość zapisywanych bajtów
  * @retval HAL_OK lub HAL_ERROR
  */
HAL_StatusTypeDef eeprom_write(struct eeprom *self, uint32_t memory_address, uint8_t *data, uint8_t data_size)
{
	uint8_t eeprom_temp_adr = fill_device_adr(self, memory_address); //pierwsze 1/2/3 bity adresu pamięci mogą być umieszczone już w adresie urządzenia (zalezy od modelu)
	if(self->eeprom_adr_len == 1) memory_address &= 0xFF;
	else memory_address &= 0xFFFF; //wyzerowanie wszystkiego poza 2 bajtami

	wp_set(self, WP_DISABLE); //write protection
	if(HAL_I2C_Mem_Write(self->eeprom_handle, eeprom_temp_adr, memory_address, self->eeprom_adr_len, data, data_size, HAL_MAX_DELAY) != HAL_OK)
	{
		wp_set(self, WP_ENABLE);
		return HAL_ERROR;
	}
	else 
	{
		wp_set(self, WP_ENABLE);
		return HAL_OK;
	}
}

/** @brief Odczyt danych z pamięci EEPROM
  * @param self: Wskażnik na strukturę pamięci EEPROM
  * @param memory_address: Adres odczytywanego słowa
  * @param data: Wskaźnik na miejsce gdzie dane zostaną zapisane
  * @param data_size: Ilość odczytywanych bajtów
  * @retval HAL_OK lub HAL_ERROR
  */
HAL_StatusTypeDef eeprom_read(struct eeprom *self, uint32_t memory_address, uint8_t *data, uint8_t data_size)
{
	uint8_t eeprom_temp_adr = fill_device_adr(self, memory_address);
	if(self->eeprom_adr_len == 1) memory_address &= 0xFF;
	else memory_address &= 0xFFFF;

	return HAL_I2C_Mem_Read(self->eeprom_handle, eeprom_temp_adr, memory_address, self->eeprom_adr_len, data, data_size, HAL_MAX_DELAY);
}