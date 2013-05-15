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
#define ID 0x01

#define SWREAD PORTbits.RB5
#define SWWRITE PORTbits.RB4
#define LED PORTCbits.RC2



/*****************************************************
		Function Prototypes
**************************************************** */

void initChip();
void InterruptHandlerHigh(void);

void ReadGPS(void);
void WriteXbee(const ram char *data);
void WriteTest(static const rom char*);
void WriteData(void);
void WriteGPS(char[]);
void initGPS(void);
void read_gps_2( void );
void handleCommand(void);


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
int sw_flag = 0;
char id[2];


unsigned char gpsReceive = 0;

/*********************************************************
	Interrupt Handler
**********************************************************/
#pragma interrupt InterruptHandlerHigh
void InterruptHandlerHigh(void)
{
    INTCONbits.GIE = 0;

        
       handleCommand();
		INTCONbits.RBIF = 0;






    
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
	TRISB = 0x30;
	PORTC = 0x00;
	TRISC = 0xC0;

	PORTAbits.RA3 = 1;
	PORTAbits.RA4 = 1;
	PORTAbits.RA5 = 1;
        


		INTCONbits.RBIE = 1;
		INTCONbits.RBIF = 0;
        RCONbits.IPEN = 1;                  //Set to priority mode
        INTCONbits.GIE = 0; //DISABLED               //Enable high priority Interrupts
        INTCONbits.PEIE = 0;                //Enable low priotity interrupts

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

RCONbits.RCIE = 0;
INTCONbits.GIE = 1;                //Enable high priority Interrupts
INTCONbits.PEIE = 1;



while(1){

   	
	read_gps_2();
	//WriteData();

    }
}

void handleCommand(void){

	char comm[20];
	int i;
	getsUART(comm, 20);

	//if(comm[0] == '$' && comm[1] == ID){
		WriteXbee(&comm);

		switch(comm[2]){

			case 'a': // Set ID command
				id[0] = comm[2];
				id[1] = comm[3];
				break;
			
			case 'b': // Initialize GPS
				initGPS();
				break;
			
			case 'c':	// Verify real device
				break;
			
			case 'd': // Ack device working
				break;
			
			case 'e': // Flicker LED
				
				for(i = 0; i < 200; i++){
					LED != LED;
					Delay10KTCYx(500000000000000);
        			Delay10KTCYx(500000000000000);

				}
				break;

	}
}



void WriteXbee(const ram char *data){
    do 
		{ // Transmit a byte 
		WriteUSART(*data); 

		while(BusyUSART());

		} while( *data++ != '\n' ); 
	} 

void WriteGPS(char bufferGPS[]){
	char buffer[100];
     putsUART(&bufferGPS);
	 memset(buffer,0,100);
}


void WriteTest(static const rom char *GPS){
    do{
        WriteUSART(*GPS);
        
        while(BusyUSART());

        } while( *GPS++ != '\n' );

       Delay10KTCYx(500000000000000);
        Delay10KTCYx(500000000000000);
      
          

}

void read_gps_2( void ){
	char buffer[100];
	int id_sent = 0;
	int i = 0;
	char c;

	buffer[0] = '$';
	buffer[1] = '0';
	buffer[2] = '1';
	buffer[3] = '\n';
	//memset(buffer,0,100);
	while(i < 100){
		PORTAbits.RA3 = !PORTAbits.RA3;

		while(!RCONbits.RCIF){
			PORTAbits.RA5 = !PORTAbits.RA5;
		
	}
		if(!id_sent){
			WriteTest("$01");
			memset(buffer,0,100);
			id_sent = 1;
		}

		c = RCREG;
		WriteUSART(c);
		

		if(c == 0xa){ // check if char is carriage return
			break;
		}

		i++;
	}
		id_sent = 0;
		i=0;
	

}

void WriteData(){
	
	char data[100];

WriteTest("$01$GPGGA,081813,5052.5396,N,00442.4935,E,1,7,49.0,101.9000015258789,M,,,,*1e\r\n");
WriteTest("$01$GPGGA,081814,5052.5397,N,00442.4942,E,1,7,42.0,101.30000305175781,M,,,,*2f\r\n");
WriteTest("$01$GPGGA,081819,5052.5395,N,00442.4944,E,1,8,24.0,99.9000015258789,M,,,,*25\r\n");
WriteTest("$01$GPGGA,081823,5052.5322,N,00442.5031,E,1,8,16.0,81.4000015258789,M,,,,*2f\r\n");
WriteTest("$01$GPGGA,081824,5052.5336,N,00442.4996,E,1,8,14.0,89.0999984741211,M,,,,*27\r\n");
WriteTest("$01$GPGGA,081827,5052.5335,N,00442.4999,E,1,8,17.0,88.5999984741211,M,,,,*2f\r\n");
WriteTest("$01$GPGGA,081829,5052.5335,N,00442.4999,E,1,8,17.0,88.9000015258789,M,,,,*2c\r\n");

}
     
	

