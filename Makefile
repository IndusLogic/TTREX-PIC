# MPLAB IDE generated this makefile for use with GNU make.
# Project: uart.mcp
# Date: Thu May 09 17:11:39 2013

AS = MPASMWIN.exe
CC = mcc18.exe
LD = mplink.exe
AR = mplib.exe
RM = rm

uart.cof : main.o
	$(LD) /l"C:\MCC18\lib" "18f2550_GT_CHIP.lkr" "main.o" /u_CRUNTIME /u_DEBUG /z__MPLAB_BUILD=1 /z__MPLAB_DEBUG=1 /o"uart.cof" /M"uart.map" /W

main.o : UART.X/main.c ../../../../../MCC18/h/delays.h ../../../../../MCC18/h/timers.h ../../../../../MCC18/h/usart.h UART.X/main.c ../../../../../MCC18/h/p18cxxx.h ../../../../../MCC18/h/p18f2550.h ../../../../../MCC18/h/pconfig.h
	$(CC) -p=18F2550 "UART.X\main.c" -fo="main.o" -D__DEBUG -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-

clean : 
	$(RM) "main.o" "uart.cof" "uart.hex"

