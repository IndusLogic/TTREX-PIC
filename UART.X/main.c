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
void WriteTest(void);
void ReadingUART(void);
void WritingUART(void);
void read_gps_2( void );

#pragma config WDT = OFF

extern void _startup (void);
//#pragma code _RESET_INTERRUPT_VECTOR = 0x000000
void _reset( void )
{

_asm goto _startup _endasm

}
/*************************************************
  RESET VECTORS: REMOVE IF NOT USING BOOTLOADER!!!
**************************************************/
//extern void _startup (void);
//#pragma code _RESET_INTERRUPT_VECTOR = 0x000800
//void _reset (void) {
//_asm goto _startup _endasm
//}
//#pragma code

#pragma code high_vector = 0x000800
void _high_ISR (void)
{
  InterruptHandlerHigh();
}

#pragma interrupt InterruptHandlerHigh


#pragma code low_vector=0x0018
void _low_ISR (void)
{
  InterruptHandlerHigh();
}

#pragma interruptlow InterruptHandlerHigh


#pragma code
/* END OF VECTOR REMAPPING*/



/*********************************************************
 Global Variables
**********************************************************/

unsigned int DelayCount;
char buff[200];

unsigned char gpsReceive = 0;

/*********************************************************
	Interrupt Handler
**********************************************************/
#pragma interrupt InterruptHandlerHigh
void InterruptHandlerHigh(void)
{
    INTCONbits.GIE = 0;
    
        PIR1bits.RCIF = 0;
       //PORTAbits.RA5 = 0;
       ReadGPS();
       WriteXbee();



        
    

    PIE1bits.RCIE= 1;
    INTCONbits.PEIE = 1;                //Enable low priotity interrupts
    INTCONbits.GIE = 1;
   	
}

#pragma code page

/*************************************************
			Initialize the CHIP
**************************************************/
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


        RCONbits.IPEN = 1;                  //Set to priority mode

        IPR1bits.RCIP=1;                    //USART Receive Interrupt Priority 0 = Low priority

        //PIR1bits.RCIF = 0;                  //The flag for the USART_RX interrupt needs to be set to zero.
        PIE1bits.RCIE= 1;
        INTCONbits.GIE = 0; //DISABLED               //Enable high priority Interrupts
        INTCONbits.PEIE = 0;                //Enable low priotity interrupts

        PORTAbits.RA5 = 1;
         PORTAbits.RA4 = 1;
          PORTAbits.RA3 = 1;

}

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



/*************************************************
                        MAIN
**************************************************/

void main(void)
{



initChip();

OpenUART();
OpenUSART( USART_TX_INT_OFF &
   USART_RX_INT_OFF &
   USART_ASYNCH_MODE &
   USART_EIGHT_BIT &
   USART_CONT_RX &
   USART_BRGH_HIGH, 1200 );

BAUDCONbits.BRG16 = 1;

INTCONbits.GIE = 1;                //Enable high priority Interrupts
INTCONbits.PEIE = 1;



while(1){
    
     read_gps_2();
      
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
        WriteUSART(buff[i]);
        
        while(BusyUSART());

        } while( buff[i] != '\0' );
  }

void ReadingUART(){
    int i = 0;
	char current_char;
    do
    {
        current_char = ReadUSART();
		buff[i] = current_char;
		i++;

    } while(current_char != '\n');
}

void UARTTest(){

    char buffer[100];
	int i = 3;
	char c;

	buffer[0] = '$';
	buffer[1] = '0';
	buffer[2] = '1';
	//memset(buffer,0,100);
	while(i < 100){





		c = ReadUART();
		//WriteUSART(c);

		buffer[i] = c ;

		if(c == 0xa){ // check if char is carriage return

		putsUART(&buffer);
		memset(buffer,0,100);
		i=0;
		break;
		}

		i++;
	}

}



void WriteTest(){
    char GPS [50] = "Hallo, dit is een test\0";
    int i = 0;

  do{
        WriteUSART(GPS[i]);
        i++;
        while(BusyUSART());
        
       } while( GPS[i] != '\0' );

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

		c = ReadUSART();	
		//WriteUSART(c);
		
		buffer[i] = c ;

		if(c == 0xa){ // check if char is carriage return
	
		putsUSART(&buffer);
		memset(buffer,0,100);
		i=0;
		break;
		}
	
		i++;
	}

}



