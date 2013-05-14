/*
 * File:   sw_uart.c
 * Author: Dries
 *
 * Created on 12 maart 2013, 14:49
 */

/*****************************************************
		Includes & Defines
**************************************************** */

#include <p18cxxx.h>
#include <delays.h>
#include <timers.h>
#include <usart.h>
#include <sw_uart.h>
#include <string.h>


#define BUF_SIZE 32
#define ID 0x55




/*****************************************************
		Function Prototypes
**************************************************** */

void initChip();
void InterruptHandlerHigh(void);

void ReadGPS(void);
void WriteXbee(void);
void WriteTest(static const rom char*);
void WriteData(void);
void WriteGPS(char[]);
void initGPS(void);
void read_gps_2( void );



/*************************************************
  RESET VECTORS: REMOVE IF NOT USING BOOTLOADER!!!
**************************************************/


extern void _startup (void);        
#pragma code _RESET_INTERRUPT_VECTOR = 0x000800
void _reset (void)
{
    _asm goto _startup _endasm
}
#pragma code

#pragma code _HIGH_INTERRUPT_VECTOR = 0x000808
void _high_ISR (void)
{
   InterruptHandlerHigh();
}


#pragma code _LOW_INTERRUPT_VECTOR = 0x000818
void _low_ISR (void)
{
   InterruptHandlerHigh();
} 
#pragma code
/* END OF VECTOR REMAPPING*/


/*********************************************************
 Global Variables
**********************************************************/

unsigned int DelayCount;
char buff[200];
char instr[5][5];


unsigned char gpsReceive = 0;

/*********************************************************
	Interrupt Handler
**********************************************************/
#pragma interrupt InterruptHandlerHigh
void InterruptHandlerHigh(void)
{
    INTCONbits.GIE = 0;

        
       read_gps_2();
		PIR1bits.RCIF = 0;






    
                    //Enable low priotity interrupts
    INTCONbits.GIE = 1;

}

#pragma code page

/*************************************************
			Initialize the CHIP
**************************************************/


/* DelayTXBitUART for sw_uart fucntions */
void DelayTXBitUART(void)
{
	/* delay for ((((2*Fosc)/(4*baud))+1)/2) - 12 cycles */
	/* dleay for ((((2*20,000,000)/(4*9600))+1)/2)-12 ~= 509 cycles */
	Delay100TCYx(12);
        Delay10TCYx(3);
        Nop();
	Nop();
        Nop();
	Nop();
        Nop();
	Nop();
        Nop();
	Nop();

}


/* DelayRXHalfBitUART for sw_uart fucntions */
void DelayRXHalfBitUART(void)
{
	/* delay for ((((2*Fosc)/(8*baud))+1)/2)-9 cycles */
	/* delay for ((((2*20,000,000)/(8*9600))+1)/2)-9 = 252 cycles */
	Delay100TCYx(6);
        Delay10TCYx(1);
	Nop();
	Nop();
        Nop();
	Nop();
        Nop();
	Nop();
}


/* DelayRXBitUART for sw_uart fucntions */
void DelayRXBitUART(void)
{
	/* delay for ((((2*Fosc)/(4*baud))+1)/2) - 14 cylces */
	/* delay for ((((2*20,000,000)/(4*9600))+1)/2)-14 ~= 507 cycles */
	Delay100TCYx(12);
        Delay10TCYx(3);
        Nop();
	Nop();
        Nop();
	Nop();
        Nop();
	Nop();

}

void initChip(){
    ADCON1 = 0x00;		//Turn off ADcon
	CMCON = 0x00;		//Turn off Comparator
	PORTA = 0x00;
	TRISA = 0x00;
	PORTB = 0x00;
	TRISB = 0xF0;
	PORTC = 0x00;
	TRISC = 0x00;
        TRISCbits.TRISC6 = 1;
        TRISCbits.TRISC7 = 1;
	PORTCbits.RC1 = 1;


        RCONbits.IPEN = 1;                  //Set to priority mode

        IPR1bits.RCIP=1;                    //USART Receive Interrupt Priority 0 = Low priority

        PIR1bits.RCIF = 0;                  //The flag for the USART_RX interrupt needs to be set to zero.
        PIE1bits.RCIE= 1;
        INTCONbits.GIE = 0; //DISABLED               //Enable high priority Interrupts
        INTCONbits.PEIE = 0;                //Enable low priotity interrupts

        PORTAbits.RA5 = 1;
         PORTAbits.RA4 = 1;
          PORTAbits.RA3 = 1;

}

void initGPS(){


	int i = 0;
	int maxInstr;

	for(i = 0; i<maxInstr; i++){
		WriteGPS(instr[i]);
	}
}



/*************************************************
                        MAIN
**************************************************/

void main(void)
{



initChip();
//initGPS();

OpenUART();
OpenUSART( USART_TX_INT_OFF &
   USART_RX_INT_ON &
   USART_ASYNCH_MODE &
   USART_EIGHT_BIT &
   USART_CONT_RX &
   USART_BRGH_HIGH, 1200 );

BAUDCONbits.BRG16 = 1;

INTCONbits.GIE = 1;                //Enable high priority Interrupts
INTCONbits.PEIE = 1;



while(1){

    //WriteTest();
	//read_gps_2();
	WriteData();

    }
}

void ReadGPS(){
	int i = 0;
	char current_char;
    do
    {
        current_char = ReadUSART();
		buff[i] = current_char;
		i++;

    } while(current_char != '\n');


}


void WriteXbee(){
    int i = -1;
   do
        { // Transmit a byte
        i++;
        putsUART(&buff);


        } while( buff[i] != '\0' );
  }

void WriteGPS(char bufferGPS[]){
	char buffer[100];
     putsUSART(&bufferGPS);
	 memset(buffer,0,100);
}


void WriteTest(static const rom char *GPS){
    int i = 0;
     do{
        WriteUSART(*GPS);
        
        while(BusyUSART());

        } while( GPS++ != '\0' );

       Delay10KTCYx(500000000000000);
        Delay10KTCYx(500000000000000);
         Delay10KTCYx(500000000000000);
          Delay10KTCYx(500000000000000);
           Delay10KTCYx(500000000000000);

}

void read_gps_2( void ){
	char buffer[100];
	int i = 3;
	char c;

	buffer[0] = '$';
	buffer[1] = '0';
	buffer[2] = '1';
	//memset(buffer,0,100);
	while(i < 100){


		
		while(!DataRdyUSART());

		PORTAbits.RA5 = PIR1bits.RCIF;

		//c = ReadUSART();
		c = RCREG;
		WriteUSART(c);
		

		//buffer[i] = c ;
/*
		if(c == 0xa){ // check if char is carriage return

		putsUSART(&buffer);
		memset(buffer,0,100);
		i=0;
		break;
		}
*/
		i++;
	}
	PIR1bits.RCIF = 0;
//putsUSART(&buffer);

}

void WriteData(){
	
	char data[100];

WriteTest("$GPGSV,3,2,12,32,27,088,21,17,26,239,14,10,20,276,,31,19,038,21*71\0");
WriteTest("$GPGSV,3,3,12,01,17,149,22,02,10,314,,25,02,353,,41,,,*41\0");
WriteTest("$GPRMC,091512.000,A,5058.4376,N,00505.6884,E,0.43,302.36,170313,,,A*68\0");
WriteTest("$GPGGA,091513.000,5058.4376,N,00505.6883,E,1,8,1.01,21.9,M,47.4,M,,*6D\0");
WriteTest("$GPGSA,A,3,32,01,31,13,17,23,04,20,,,,,1.30,1.01,0.83*0A\0");
WriteTest("$GPGSV,3,1,11,23,79,136,24,13,59,202,21,20,53,094,22,04,46,297,18*71\0");
WriteTest("$GPGSV,3,2,11,32,27,088,20,17,26,239,16,10,20,276,,31,19,038,20*70\0");
WriteTest("$GPGSV,3,3,11,01,17,149,21,02,10,314,,25,02,353,*44\0");
WriteTest("$GPRMC,091513.000,A,5058.4376,N,00505.6883,E,0.39,302.36,170313,,,A*63\0");
WriteTest("$GPGGA,091514.000,5058.4376,N,00505.6881,E,1,8,1.00,21.9,M,47.4,M,,*69\0");
WriteTest("$GPGSA,A,3,32,01,31,13,17,23,04,20,,,,,1.30,1.00,0.83*0B\0");
WriteTest("$GPGSV,3,1,11,23,79,136,24,13,59,202,22,20,53,094,23,04,46,297,18*73\0");
WriteTest("$GPGSV,3,2,11,32,27,088,20,17,26,239,16,10,20,276,,31,19,038,21*71\0");
WriteTest("$GPGSV,3,3,11,01,17,149,21,02,10,314,,25,02,353,*44\0");
WriteTest("$GPRMC,091514.000,A,5058.4376,N,00505.6881,E,0.58,302.36,170313,,,A*61\0");
WriteTest("$GPGGA,091515.000,5058.4378,N,00505.6879,E,1,8,1.01,21.9,M,47.4,M,,*60\0");
WriteTest("$GPGSA,A,3,32,01,31,13,17,23,04,20,,,,,1.30,1.01,0.83*0A\0");
WriteTest("$GPGSV,3,1,11,23,79,136,24,13,59,202,22,20,53,094,23,04,46,297,17*7C\0");
WriteTest("$GPGSV,3,2,11,32,27,088,19,17,26,239,16,10,20,276,,31,19,038,21*7B\0");
WriteTest("$GPGSV,3,3,11,01,17,149,21,02,10,314,,25,02,353,*44\0");



}
     
	

