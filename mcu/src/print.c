#include "print.h"

#include "globals.h"
#include "serial.h"


void prints(char *data){
	while (*data){
		if( *data == '\n' )
			xSerialPutChar('\r');
		xSerialPutChar(*data);
		data++;
	}	
}

void printc(char c){
	//sputchar(c);
	xSerialPutChar(c);
}

void printh8(uint8_t num){//while 3910bytes //for 3894 //3816
	int8_t i;
	uint8_t b;
		i = 8;
	while (i>0){
		i -= 4;
		b = num >> i;
		b &= 0x0F;
		if (b < 0x0A){
			b += 0x30;
		}else{
			b += 0x37;
		}
		//sputchar(b);
		xSerialPutChar(b);
	}	
}

void printh16(uint16_t num){
	uint8_t i;
	i = 16;
	while(i>0){
		i-=8;
		printh8((num >> i) & 0xFF);
		
	}
}
#if 1
void printh32(uint32_t num){
	uint8_t i;
	i = 32;
	while(i>0){
		i-=8;
		//num = num >> i;
		printh8((num >> i) & 0xFF);
		
	}
}
#endif

