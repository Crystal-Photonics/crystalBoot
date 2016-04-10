/*
 *  blink.c
 *  ATtiny2313 mit 1 MHz
 *  PORTB wird ueber ein Timer alle 0.263s ein- und ausgeschaltet. Das entspricht 3.81Hz
 */
/*
 * 

Today we shall be playing with Ubuntu Gutsy Gibbon and a dodgy bluetooth GPS gizmo bought for twenty odd quid on eBay….

First of all, note the extremely poor quality of the device and the way the power button sticks in when you press it too hard. Secondly, note the way that the ‘earth’ pin on the three pin socket adaptor snaps off in the wall. Hoorah for cheap electronics!

Right, on to the interesting bit. To start with, you’ll need to find the address of your new gizmo:

hcitool scan

You should get one or more device listings returned. Note the MAC address registered to your bluetooth GPS device.

00:11:67:80:xx:xx iNav GPS

Taking the address, you now need to create a serial port connection to your GPS device.

sdptool browse 00:11:67:80:xx:xx

Which then returns output describing your new shiny virtual serial port.

And then by editing this file.

sudo gedit /etc/bluetooth/rfcomm.conf

And adding this text.

rfcomm4 {
bind yes;
device < YOUR GPS DEVICES MAC ADDRESS>;
channel 1;
comment “Serial Port”;
}

You artfully create a way to invoke your serial port quickly and painlessly by typing:

rfcomm connect 4*/
#define __DELAY_BACKWARD_COMPATIBLE__

#include <avr/interrupt.h>
#include <stdint.h>
#include "board.h"
#include "print.h"
#include "serial.h"

#include "led.h"
#include "globals.h"
#include "max6675.h"
#include <util/delay.h>
#include "solidstatepwm.h"

#include "channel_codec/channel_codec.h"
#include "errorlogger/generic_eeprom_errorlogger.h"
#include "rpc_transmission/client/generated_app/RPC_TRANSMISSION_mcu2qt.h"
uint8_t TARGETEMP_C = 55;
const uint8_t P_CONST_DIV_BY_16 = 16;
const int8_t TEMPERATURE_TOLERACE = 4;

//setting fuses:avrdude -p atmega8 -P /dev/ttyUSB0     -c avr910    -b115200 -U lfuse:w:0xee:m -U hfuse:w:0xd9:m 

//t_task_data_sys task_data_sys;


void ChannelCodec_errorHandler(channelCodecErrorNum_t ErrNum){
	(void)ErrNum;
	
}


int main(void) __attribute__ ((noreturn));


 
int main(void)
{
	uint16_t timebaselo = 0;
	uint8_t timebaseled = 0;
	int16_t temperature = 0;
	int8_t result = 0;

	initPort();

	//task_mot_init(&task_data_mot);
	xSerialPortInitMinimal( serBAUD19200 );
	task_led_ini(&led_data);
	spwm_init();
	//suart_init();
	sei(); // Interrupts einschalten
	//sputchar('A');
	max6675Init();
	temperature = 40;
	max33185result_t temperature_data;
	channel_init();
	while (1){
		int16_t errorvalue;
		uint8_t inByte;
		while (xSerialGetChar(&inByte)){
			channel_push_byte_to_RPC(inByte);
		}
		timebaselo++;

		_delay_ms(50);
		CLEAR_LED_L();
		//CLEAR_BUZZER();
		result= max6675Read(&temperature_data);
		temperature = temperature_data.temperature_thermocuouple/4;
		(void)result;
		errorvalue = TARGETEMP_C-temperature;

		if ((TEMPERATURE_TOLERACE > errorvalue) && (errorvalue > -TEMPERATURE_TOLERACE) ){
			SET_LED_M();
		}else{
			CLEAR_LED_M();
		}

		//errorvalue += 2;

		errorvalue *=P_CONST_DIV_BY_16;

		errorvalue/=16;
		if (errorvalue < 0){
			errorvalue = 0;
		}

		//printc(' ');
		//printh16(temperature/4);
		//printc(' ');
		//printh16(errorvalue);
		//printc('\n');
		//
#if 1
		qtUpdateRealTemperature(
				temperature_data.temperature_thermocuouple,
				temperature_data.temperature_coldjunction,
				TARGETEMP_C,errorvalue);
#endif
		printc('\n');

		spwm_setVal(errorvalue);

		_delay_ms(50);



		timebaseled+=1;
		SET_LED_L();

		//SET_BUZZER();


	}

#if 0
 while(1){
	 timebaseled++;
		if ((timebaseled&1) == 0)
			SET_BT_LED();
		if (timebaseled&1)
			CLEAR_BT_LED();
		 _delay_ms(500);
 }
#endif

	do{
		timebaselo++;



#if 0
		if (((timebaselo) & TASK_UI_SPEED_MASK_LO) == 0){
			task_ui_trigger(&task_data_ui);
		}
#endif
	}while(1);
	//return 0;
}

