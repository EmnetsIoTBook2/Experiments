/*********************************************************************************
  *FileName: dht11register.c
  *Author:  SunTong
  *Version: v1.0.1
  *Date: 2021/12/22
  *Description: 树莓派linux下直接控制寄存器进行pin脚的读写和上拉，
  				实现dht11的驱动
  				因为 linux下的 usleep函数不准，所以使用wiringPi里的延时函数
  *History:
**********************************************************************************/


#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <unistd.h>
#include <wiringPi.h>

#define GPIO_BASE_Physical_Address	0x3f200000

#define GPFSEL1_Offs 0x04
#define GPSET0_Offs 0x1c
#define GPCLR0_Offs 0x28
#define GPLEV0_offs 0x34

#define GPPUD_Offs 0x94
#define GPPUDCLK0_Offs 0x98

//typedef unsigned char uint8_t;

typedef enum {
	GPIO_FSEL_INPT = 0x00,
	GPIO_FSEL_OUTP = 0x01
} FunctionSelect_GPFSEL;

void *GPIO_BASE;
volatile uint32_t * GPFSEL1;
volatile uint32_t * GPSET0;
volatile uint32_t * GPCLR0;
volatile uint32_t * GPPUD;
volatile uint32_t * GPPUDCLK0;
volatile uint32_t * GPLEV0;

int initialGpio(void) {
	int fd;
    fd = open("/dev/gpiomem", O_RDWR);
    if (fd == -1)   
    {
        printf("open Error!\n");
        return -1;
    }

    GPIO_BASE = mmap(0, sysconf(_SC_PAGESIZE), PROT_READ | PROT_WRITE, MAP_SHARED , fd, GPIO_BASE_Physical_Address);
    close(fd);
    if(GPIO_BASE == MAP_FAILED)
    {
        printf("mmap Error!\n");
        return -1;
    }
	GPFSEL1 = (uint32_t *)(GPIO_BASE + GPFSEL1_Offs);
 	GPSET0 = (uint32_t *)(GPIO_BASE + GPSET0_Offs);
	GPCLR0 = (uint32_t *)(GPIO_BASE + GPCLR0_Offs);
	GPLEV0 = (uint32_t *)(GPIO_BASE + GPLEV0_offs);
	GPPUD = (uint32_t *)(GPIO_BASE + GPPUD_Offs);
	GPPUDCLK0 = (uint32_t *)(GPIO_BASE + GPPUDCLK0_Offs);	
}

void set_gpio_pin_Mode(uint8_t pin, FunctionSelect_GPFSEL functionCode) {
    pin = pin % 10;
	if (functionCode == GPIO_FSEL_INPT) {
		*GPFSEL1 = *GPFSEL1 & ~((uint32_t)7 << (3 * pin));
	} else if (functionCode == GPIO_FSEL_OUTP) {
		*GPFSEL1 = (*GPFSEL1 & ~((uint32_t)7 << (3 * pin))) | ((uint32_t)1) << (3 * pin);
	}
}

void my_digitalWrite(uint8_t pin, uint8_t value) {
	if (value == 1) {
		*GPCLR0 = ((uint32_t)1) << pin;
	} else {
		*GPSET0 = ((uint32_t)1) << pin;
	}
}

int my_digitalRead(uint8_t pin) {
	if ((*GPLEV0 & (1 << (pin & 31))) != 0) {
		return 1;
	} else {
		return 0;
	}
}

void pullUpControl(uint8_t pin) {
	*GPPUD = (uint32_t)(2 & 3) ;
    delayMicroseconds(5);
	*GPPUDCLK0 = ((uint32_t)1) << pin;
    delayMicroseconds(5);
	*GPPUD = (uint32_t)0 ;
    delayMicroseconds(5);;
	*GPPUDCLK0 = (uint32_t)0 ;
    delayMicroseconds(5);
}



/********************************************************************************************/
uint8_t pinNumber = 18;
uint32_t databuf;

uint8_t readSensorData(void) {
	uint8_t crc;
	uint8_t i;
	set_gpio_pin_Mode(pinNumber, GPIO_FSEL_OUTP); // set pin mode to output
	my_digitalWrite(pinNumber, 1); // output a high level
    delayMicroseconds(4); // delay 4 us
	my_digitalWrite(pinNumber, 0);
	delay(25); // delay 25 ms
	digitalWrite(pinNumber, 1); // output a high level
    delayMicroseconds(60);
	set_gpio_pin_Mode(pinNumber, GPIO_FSEL_INPT); // set pin mode to input
	pullUpControl(pinNumber); // pull up

	//delayMicroseconds(27);
	if (my_digitalRead(pinNumber) == 0)
	{
		while (!my_digitalRead(pinNumber))
			; // wait to high
        delayMicroseconds(80);
		for (i = 0; i < 32; i++) {
			while (my_digitalRead(pinNumber))
				; // data clock start
			while (!my_digitalRead(pinNumber))
				; // data start
            delayMicroseconds(32);
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
            delayMicroseconds(32);
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




int main()
{
	initialGpio();
	
	printf("PIN:%d\n", pinNumber);
	
	set_gpio_pin_Mode(pinNumber, GPIO_FSEL_OUTP);
	my_digitalWrite(pinNumber, 1);

	printf("Starting...\n");
	while (1) {
		set_gpio_pin_Mode(pinNumber, GPIO_FSEL_OUTP);
		my_digitalWrite(pinNumber, 1);
	    sleep(1);	
		if (readSensorData()) {
			printf("Sensor data read ok!\n");
			printf("RH:%d.%d\n", (databuf >> 24) & 0xff, (databuf >> 16) & 0xff);
			printf("TMP:%d.%d\n", (databuf >> 8) & 0xff, databuf & 0xff);
			databuf = 0;
		} else {
			printf("Sensor doesent ans!\n");
			databuf = 0;
		}
	}

    if(munmap(GPIO_BASE, sysconf(_SC_PAGESIZE)) == -1)
    {
        printf("munmap Error!\n");
        return -1;
    }
//    GPIO_BASE = MAP_FAILED;
//    GPFSEL0 = MAP_FAILED;
//    GPSET0 = MAP_FAILED;
//    GPCLR0 = MAP_FAILED;
	printf("you succeed!\n");
    return 0;
}

