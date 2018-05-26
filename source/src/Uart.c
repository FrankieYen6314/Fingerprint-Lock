#include "Uart.h"

/* Public variables ----------------------------------------------------------*/
volatile bool g_bUartFlag =false;
uint8 UartBuffer[26];

void UartInit()
{
	P1SEL = BIT1 + BIT2;  // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2; // P1.1 = RXD, P1.2=TXD
    UCA0CTL1 |= UCSSEL_2; // SMCLK
	
	//UCA0BR1 = 0;        // 1MHz 9600bps
    UCA0BR0 = 0x09;       // 1MHz 115200bps
    UCA0BR1 = 0;          // 1MHz 115200
    //UCA0BR0 = 104;        // 1MHz 9600
    //UCA0MCTL = 0x02;      // Modulation UCBRSx = 1
    UCA0MCTL = 0x10;
    UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;      // Enable USCI_A0 RX interrupt
}

void SerialPutChar(uint8 Data)
{
	while(!(IFG2&UCA0TXIFG));
	UCA0TXBUF =Data;
}


#pragma vector=USCIAB0RX_VECTOR //接收完一个完整字节后产生串口UART0中断
__interrupt void USCI0RX_ISR(void)
{
	static uint8 buffer_counter = 0;	//Buffer Counter of UART
	
	if(UCA0RXBUF == 0xAA)
	{
		UartBuffer[0] =UCA0RXBUF;
		buffer_counter =1;
	}
	if(UartBuffer[0] ==0xAA && UCA0RXBUF != 0xAA)
	{
		UartBuffer[buffer_counter] =UCA0RXBUF;
		if(buffer_counter ==25)
			g_bUartFlag =true;
		buffer_counter ++;
	}
}