/*****************************************************************************
File name: pta_Rpi3dht11.c
Description: dht11(temperature and humidity sensor) driver for Rpi3 in optee os kernel
Author: Sun Tong
Version: 1.0
Date: 2021/12/22
History: 
*****************************************************************************/
#include <stdio.h>
#include <trace.h>
#include <string.h>
#include <kernel/delay.h>
#include <kernel/pseudo_ta.h>
#include <io.h>

#include <mm/core_mmu.h>
#include <mm/core_memprot.h>
#include <mm/tee_pager.h>
#include <mm/tee_mm.h>

#define TA_NAME "pta_Rpi3dht11.ta"
//bd867e69-58fb-4675-8326-5a68a3bfcc7f
#define PTA_RPI3DHT11_UUID \
	{ 0xbd867e69, 0x58fb, 0x4675, \
		{ 0x83, 0x26, 0x5a, 0x68, 0xa3, 0xbf, 0xcc, 0x7f}}
/****************************************
 *invoke cmd
 *****************************************/
#define GPIO_ON					0
#define GPIO_OFF				1
#define TEST_PSEUDO_TA 			2
#define DHT11_READ_DATA			3

/************************************************
 *gpio physical address
 *because we use pin18, so selcect GPFSEL1
 *************************************************/
#define RPI3_GPIO_BASE_Physical_Address		0x3F200000

#define GPFSEL1_Offs 0x04
#define GPSET0_Offs 0x1c
#define GPCLR0_Offs 0x28
#define GPLEV0_offs 0x34

#define GPPUD_Offs 0x94
#define GPPUDCLK0_Offs 0x98


typedef enum {
	GPIO_FSEL_INPT = 0x00,
	GPIO_FSEL_OUTP = 0x01
} FunctionSelect_GPFSEL;

vaddr_t GPIO_BASE;
vaddr_t GPFSEL1;
vaddr_t GPSET0;
vaddr_t GPCLR0;
vaddr_t GPPUD;
vaddr_t GPPUDCLK0;
vaddr_t GPLEV0;

/**************************************************
 *gpio register physical address to virtual address
 *************************************************/
static vaddr_t nsec_periph_base(paddr_t pa) {
	if (cpu_mmu_enabled()) {
		return (vaddr_t)phys_to_virt(pa, MEM_AREA_IO_NSEC);
	}
	return (vaddr_t)pa;
}

static void initializeGpio(void) {
	GPIO_BASE = nsec_periph_base((paddr_t) (RPI3_GPIO_BASE_Physical_Address));
	GPFSEL1 = GPIO_BASE + GPFSEL1_Offs;
	GPSET0 = GPIO_BASE + GPSET0_Offs;
	GPCLR0 = GPIO_BASE + GPCLR0_Offs;
	GPLEV0 = GPIO_BASE + GPLEV0_offs;
	GPPUD = GPIO_BASE + GPPUD_Offs;
	GPPUDCLK0 = GPIO_BASE + GPPUDCLK0_Offs;	
}

/**************************************************
 * set GPIO MODE: INPUT / OUTPUT
 *************************************************/
void set_gpio_pin_Mode(uint8_t pin, FunctionSelect_GPFSEL functionCode) {
	pin = pin % 10;
	if (functionCode == GPIO_FSEL_INPT) {
		uint32_t data;
		uint32_t mask = ((uint32_t)7 << (3 * pin));
		data = read32(GPFSEL1);
		data = data & ~mask;
		write32(data, GPFSEL1);

		//*GPFSEL1 = *GPFSEL1 & ~((uint32_t)7 << (3 * pin));
	} else if (functionCode == GPIO_FSEL_OUTP) {
		uint32_t data;
		uint32_t mask = ((uint32_t)7 << (3 * pin));
		data = read32(GPFSEL1);
		data = data & ~mask;
		data = data | (((uint32_t)1) << (3 * pin));
		write32(data, GPFSEL1);

		//*GPFSEL1 = (*GPFSEL1 & ~((uint32_t)7 << (3 * pin))) | ((uint32_t)1) << (3 * pin);
	}
}
/**************************************************
 * set pin output low/high level
 *************************************************/
void my_digitalWrite(uint8_t pin, uint8_t value) {
	if (value == 0) {
		uint32_t data;
		data = read32(GPCLR0);
		data = ((uint32_t)1) << pin;
		write32(data, GPCLR0);
		// *GPCLR0 = ((uint32_t)1) << pin;
	} else if (value == 1) {
		uint32_t data;
		data = read32(GPSET0);
		data = ((uint32_t)1) << pin;
		write32(data, GPSET0);
		// *GPSET0 = ((uint32_t)1) << pin;
	}
}
/**************************************************
 * read pin level
 *************************************************/
int my_digitalRead(uint8_t pin) {
	uint32_t data;
	data = read32(GPLEV0);
	if ((data & (1 << (pin & 31))) != 0) {
		return 1;
	} else {
		return 0;
	}
}
/**************************************************
 * set pin pullup
 *************************************************/
void pullUpControl(uint8_t pin) {
	uint32_t data;
	data = read32(GPPUD);
	data = (uint32_t)(2 & 3);
	write32(data, GPPUD);
	udelay(5);
	data = read32(GPPUDCLK0);
	data = ((uint32_t)1) << pin;
	write32(data, GPPUDCLK0);
	udelay(5);
	data = read32(GPPUD);
	data = (uint32_t)0;
	write32(data, GPPUD);
	udelay(5);
	data = read32(GPPUDCLK0);
	data = (uint32_t)0;
	write32(data, GPPUDCLK0);
	udelay(5);
//	*GPPUD = (uint32_t)(2 & 3) ;
//	//mdelay(5) ;
//	usleep(5000);
//	*GPPUDCLK0 = ((uint32_t)1) << (pin & 31) ;
//	usleep(5000);
//	*GPPUD = (uint32_t)0 ;
//	usleep(5000);;
//	*GPPUDCLK0 = (uint32_t)0 ;
//	usleep(5000);
}
/************************************************** TEE INVOKE Functions *************************************************/


/**************************************************
 * read dht11 sensor data
 *************************************************/

uint32_t databuf;

uint8_t readSensorData(uint8_t pinNumber) {
	uint8_t crc;
	uint8_t i;
	set_gpio_pin_Mode(pinNumber, GPIO_FSEL_OUTP); // set pin mode to output
	my_digitalWrite(pinNumber, 1); // output a high level
	udelay(4);
	my_digitalWrite(pinNumber, 0);
	mdelay(25);
	my_digitalWrite(pinNumber, 1); // output a high level
	udelay(60);
	set_gpio_pin_Mode(pinNumber, GPIO_FSEL_INPT); // set pin mode to input
	pullUpControl(pinNumber); // pull up

	if (my_digitalRead(pinNumber) == 0)
	{
		while (!my_digitalRead(pinNumber))
			; // wait to high
		udelay(80);
		for (i = 0; i < 32; i++) {
			while (my_digitalRead(pinNumber))
				; // data clock start
			while (!my_digitalRead(pinNumber))
				; // data start
			udelay(32);
			databuf *= 2;
			if (my_digitalRead(pinNumber) == 1) {
				databuf ++;
			}
		}
		for (i = 0; i < 8; i++) {
			while (my_digitalRead(pinNumber))
				;
			while (!my_digitalRead(pinNumber))
				;
			udelay(32);
			crc *= 2;
			if (my_digitalRead(pinNumber) == 1) {
				crc ++;
			}
		}
		return 1;
	} else {
		return 0;
	}
}

static TEE_Result testGpioON(uint32_t type, TEE_Param p[TEE_NUM_PARAMS]) {
	if (TEE_PARAM_TYPES(
					TEE_PARAM_TYPE_VALUE_OUTPUT,
					TEE_PARAM_TYPE_NONE,
					TEE_PARAM_TYPE_NONE,
					TEE_PARAM_TYPE_NONE) != type) {
				EMSG("Invalid arguments");
				return TEE_ERROR_BAD_PARAMETERS;
	}
	static const int LedGpioPin = 18;

	initializeGpio();

	set_gpio_pin_Mode(LedGpioPin, GPIO_FSEL_OUTP);
	my_digitalWrite(LedGpioPin, 1);
	udelay(4);
	if (my_digitalRead(LedGpioPin) == 0) {
		p[0].value.a = 0;
	} else {
		p[0].value.a = 1;
	}

	
	return TEE_SUCCESS;
}

static TEE_Result testGpioOFF(uint32_t type, TEE_Param p[TEE_NUM_PARAMS]) {
	if (TEE_PARAM_TYPES(
					TEE_PARAM_TYPE_VALUE_OUTPUT,
					TEE_PARAM_TYPE_NONE,
					TEE_PARAM_TYPE_NONE,
					TEE_PARAM_TYPE_NONE) != type) {
				EMSG("Invalid arguments");
				return TEE_ERROR_BAD_PARAMETERS;
	}
	static const int LedGpioPin = 18;

	initializeGpio();

	set_gpio_pin_Mode(LedGpioPin, GPIO_FSEL_OUTP);
	my_digitalWrite(LedGpioPin, 0);
	udelay(4);
	if (my_digitalRead(LedGpioPin) == 0) {
		p[0].value.b = 1;
	} else {
		p[0].value.b = 0;
	}
	return TEE_SUCCESS;
}

static TEE_Result testPseudoTA(uint32_t type, TEE_Param p[TEE_NUM_PARAMS]) {
	if (TEE_PARAM_TYPES(
					TEE_PARAM_TYPE_VALUE_OUTPUT,
					TEE_PARAM_TYPE_NONE,
					TEE_PARAM_TYPE_NONE,
					TEE_PARAM_TYPE_NONE) != type) {
				EMSG("Invalid arguments");
				return TEE_ERROR_BAD_PARAMETERS;
	}
	p[0].value.a = 20;
	return TEE_SUCCESS;
}

static TEE_Result DHT11_readData(uint32_t type, TEE_Param p[TEE_NUM_PARAMS]) {
	if (TEE_PARAM_TYPES(
			TEE_PARAM_TYPE_VALUE_OUTPUT,
			TEE_PARAM_TYPE_VALUE_OUTPUT,
			TEE_PARAM_TYPE_NONE,
			TEE_PARAM_TYPE_NONE) != type) {
		EMSG("Invalid arguments");
		return TEE_ERROR_BAD_PARAMETERS;
	}
	static const int DHT11GpioPin = 18;
	initializeGpio();
	if (readSensorData(DHT11GpioPin)) {
		EMSG("Sensor data read ok!");
		// temperature data
		p[0].value.a = (uint32_t)((databuf >> 24) & 0xff);
		p[0].value.b = (uint32_t)((databuf >> 16) & 0xff);
		// humidity data
		p[1].value.a = (uint32_t)((databuf >> 8) & 0xff);
		p[1].value.b = (uint32_t)(databuf & 0xff);
		//printf("RH:%d.%d\n", (databuf >> 24) & 0xff, (databuf >> 16) & 0xff);
		//printf("TMP:%d.%d\n", (databuf >> 8) & 0xff, databuf & 0xff);
		databuf = 0;
	} else {
		EMSG("Sensor does not ans!");
		databuf = 0;
	}
	return TEE_SUCCESS;
}

static TEE_Result invoke_command(void *psess __unused, uint32_t cmd, uint32_t ptypes, TEE_Param params[TEE_NUM_PARAMS]) {
	(void)ptypes;
	(void)params;

	switch (cmd) {
		case GPIO_ON:
			return testGpioON(ptypes, params);
		case GPIO_OFF:
			return testGpioOFF(ptypes, params);
		case TEST_PSEUDO_TA:
			return testPseudoTA(ptypes, params);
		case DHT11_READ_DATA:
			return DHT11_readData(ptypes, params);
		default:
			break;
	}
	return TEE_ERROR_BAD_PARAMETERS;
}

pseudo_ta_register(.uuid = PTA_RPI3DHT11_UUID, .name = TA_NAME,
			.flags = PTA_DEFAULT_FLAGS,
			.invoke_command_entry_point = invoke_command);
