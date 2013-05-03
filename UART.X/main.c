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

/*************************************************
  RESET VECTORS: REMOVE IF NOT USING BOOTLOADER!!!
**************************************************/
extern void _startup (void);
#pragma code _RESET_INTERRUPT_VECTOR = 0x000800
void _reset (void) {
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
char buff[255] = "Hallo, Raspberry Pi\r\n";

unsigned char gpsReceive = 0;

/*********************************************************
	Interrupt Handler
**********************************************************/
#pragma interrupt InterruptHandlerHigh
void InterruptHandlerHigh(void)
{
    INTCONbits.GIE = 0;
    
        PIR1bits.RCIF = 0;
       // PORTAbits.RA5 = !PORTAbits.RA5;
        ReadGPS();
        WriteXbee();



        
    

    PIE1bits.RCIE= 1;
    INTCONbits.PEIE = 1;                //Enable low priotity interrupts
    INTCONbits.GIE = 1;


   	
}



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

        PIR1bits.RCIF = 0;                  //The flag for the USART_RX interrupt needs to be set to zero.
        PIE1bits.RCIE= 1;
        INTCONbits.GIE = 1;                //Enable high priority Interrupts
        INTCONbits.PEIE = 1;                //Enable low priotity interrupts

        PORTAbits.RA5 = 1;

}



/*************************************************
                        MAIN
**************************************************/
void main(void)
{



initChip();

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
    
     /*
      if(gpsReceive){
          PORTAbits.RA5 = !PORTAbits.RA5;
          gpsReceive = 0;
          ReadGPS;
          WriteXbee;
      }
      */
    }
}

void ReadGPS(){
    int i = 0;


    do
    {
        PORTAbits.RA5 = !PORTAbits.RA5;
        while(!DataRdyUSART());
        buff[i] = ReadUSART();
        i++;
    } while(i < 5);
    buff[i] = '\0';

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

void WriteTest(){
    char GPS [50] = "Hallo, dit is een test";
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




