#ifdef TEST
#define TEST

#include "unity.h"

#include "AT24Cxxx.h"
#include "HAL_mock.h"

struct eeprom eeprom;
I2C_HandleTypeDef hi2c1;

void setUp(void) 
{ 
  hi2c1.Id = 1;	
}
void tearDown(void) {}

void test_InitializeShouldSaveValues(void)
{
	eeprom_initialize(&eeprom, &hi2c1, C256, NO_PINS);

	TEST_ASSERT_EQUAL(0xA0, eeprom.eeprom_device_adr);
	TEST_ASSERT_EQUAL(hi2c1.Id, eeprom.eeprom_handle->Id);
	TEST_ASSERT_EQUAL(2, eeprom.eeprom_adr_len);
	TEST_ASSERT_EQUAL(C256, eeprom.eeprom_model);
	
}

void test_InitializeAddressPins(void)
{
	hi2c1.Id = 2;
	eeprom_initialize(&eeprom, &hi2c1, C04, A1);
	
	TEST_ASSERT_EQUAL(hi2c1.Id, eeprom.eeprom_handle->Id);
	TEST_ASSERT_EQUAL(1, eeprom.eeprom_adr_len);
	TEST_ASSERT_EQUAL(C04, eeprom.eeprom_model);
	TEST_ASSERT_EQUAL(0xA4, eeprom.eeprom_device_adr);
}

void test_InitializeWP(void)
{
	eeprom_initialize(&eeprom, &hi2c1, C256, NO_PINS);
	GPIO_TypeDef gpio;
	gpio.Id = 5;
	eeprom_initialize_wp(&eeprom, &gpio, 6);
	
	TEST_ASSERT_EQUAL(5, eeprom.wp_gpio->Id);
	TEST_ASSERT_EQUAL(6, eeprom.wp_gpio_pin);
	TEST_ASSERT_EQUAL(1, gpio.state);
}	

void test_DeinitializeWP(void)
{
	eeprom_initialize(&eeprom, &hi2c1, C256, NO_PINS);
	GPIO_TypeDef gpio;
	gpio.Id = 2;
	eeprom_initialize_wp(&eeprom, &gpio, 5);
	eeprom_deinitialize_wp(&eeprom);
	
	TEST_ASSERT_EQUAL(0, gpio.state);
	TEST_ASSERT_EQUAL(NULL, eeprom.wp_gpio);
	TEST_ASSERT_EQUAL(0, eeprom.wp_gpio_pin);
}	

void test_FillDeviceAdrress(void)
{
	uint8_t data = 0xB0;
	eeprom_initialize(&eeprom, &hi2c1, C256, NO_PINS); //C256 - bez uzupelnienia adresu
	eeprom_write(&eeprom, 0x0C, &data, 1);
	
	TEST_ASSERT_EQUAL(C256, eeprom.eeprom_model);
	TEST_ASSERT_EQUAL_UINT8(0xA0, hi2c1.address);
	TEST_ASSERT_EQUAL_UINT32(0x0C, hi2c1.memory_address);
	
	
	eeprom_initialize(&eeprom, &hi2c1, M01, NO_PINS); //M01 - jeden bit
	eeprom_write(&eeprom, 0x1AA0C, &data, 1);
	
	TEST_ASSERT_EQUAL(M01, eeprom.eeprom_model);
	TEST_ASSERT_EQUAL_UINT8(0xA2, hi2c1.address);
	TEST_ASSERT_EQUAL_UINT32(0xAA0C, hi2c1.memory_address);
	
	
	eeprom_initialize(&eeprom, &hi2c1, C04, NO_PINS);
	eeprom_write(&eeprom, 0x10C, &data, 1);
	
	TEST_ASSERT_EQUAL(C04, eeprom.eeprom_model);
	TEST_ASSERT_EQUAL_UINT8(0xA2, hi2c1.address);
	TEST_ASSERT_EQUAL_UINT32(0x0C, hi2c1.memory_address);
	
	
	eeprom_initialize(&eeprom, &hi2c1, M02, NO_PINS);
	eeprom_write(&eeprom, 0x2AA0C, &data, 1);
	
	TEST_ASSERT_EQUAL(M02, eeprom.eeprom_model);
	TEST_ASSERT_EQUAL_UINT8(0xA4, hi2c1.address);
	TEST_ASSERT_EQUAL_UINT32(0xAA0C, hi2c1.memory_address);
	
	
	eeprom_initialize(&eeprom, &hi2c1, C08, NO_PINS);
	eeprom_write(&eeprom, 0x1FE, &data, 1);
	
	TEST_ASSERT_EQUAL(C08, eeprom.eeprom_model);
	TEST_ASSERT_EQUAL_UINT8(0xA2, hi2c1.address);
	TEST_ASSERT_EQUAL_UINT32(0xFE, hi2c1.memory_address);
	
	
	eeprom_initialize(&eeprom, &hi2c1, C16, NO_PINS);
	eeprom_write(&eeprom, 0x6CB, &data, 1);
	
	TEST_ASSERT_EQUAL(C16, eeprom.eeprom_model);
	TEST_ASSERT_EQUAL_UINT8(0xAC, hi2c1.address);
	TEST_ASSERT_EQUAL_UINT32(0xCB, hi2c1.memory_address);
	
	
	eeprom_initialize(&eeprom, &hi2c1, C04, A1A2);
	eeprom_write(&eeprom, 0x1CB, &data, 1);
	
	TEST_ASSERT_EQUAL(C04, eeprom.eeprom_model);
	TEST_ASSERT_EQUAL(0xAC, eeprom.eeprom_device_adr);
	TEST_ASSERT_EQUAL_UINT8(0xAE, hi2c1.address);
	TEST_ASSERT_EQUAL_UINT32(0xCB, hi2c1.memory_address);
}

void test_WriteRead(void)
{
	uint8_t data = 0xB0;
	uint8_t data_r = 0x00;
	eeprom_initialize(&eeprom, &hi2c1, C256, NO_PINS);
	eeprom_write(&eeprom, 0x10AE, &data, 1);
	eeprom_read(&eeprom, 0x10AE, &data_r ,1);
	
	TEST_ASSERT_EQUAL_UINT8(data, data_r);	
}	

void test_WriteReadArray(void)
{
	uint8_t data[4] = {0x11, 0x22, 0x33, 0x44};
	uint8_t data_r[4] = {0, 0, 0, 0};
	eeprom_initialize(&eeprom, &hi2c1, C256, NO_PINS);
	eeprom_write(&eeprom, 0x2036, data, 4);
	eeprom_read(&eeprom, 0x2036, data_r , 4);

	TEST_ASSERT_EQUAL_UINT8(data[0], data_r[0]);
	TEST_ASSERT_EQUAL_UINT8(data[1], data_r[1]);
	TEST_ASSERT_EQUAL_UINT8(data[2], data_r[2]);
	TEST_ASSERT_EQUAL_UINT8(data[3], data_r[3]);
}	
#endif
