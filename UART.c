#include "UART.h"

void USART_send(unsigned char data)
{
	
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
	
}

void USART_putstring(char* StringPtr)
{
	
	while(*StringPtr != 0x00){    //Here we check if there is still more chars to send, this is done checking the actual char and see if it is different from the null char
		USART_send(*StringPtr);    //Using the simple send function we send one char at a time
	StringPtr++;}        //We increment the pointer so we can read the next char
	
}

int USARTReadChar()
{
	unsigned int control=40000;//40000
	//Wait untill a data is available
	while(!(UCSR0A & (1<<RXC0)) && (control--));
	//Now USART has got data from host
	//and is available is buffer
	return UDR0;
}


char USARTReadString(char data[], char len, char header[])
{
	unsigned char i=0, aux=0, y=0, n=0, x=0;
	
	while ((i<len) & (y<20) & (n<3) & (x<2))//14
	{
		aux = USARTReadChar();
		if ((i>=4) | (aux==header[i]))
		{
			data[i]=aux;
			i++;
		}else if (aux==no_data[n])
		{
			n++;
		}else if (aux==conecting[x])
		{
			x++;
		}
		y++; 
	}
	
	if (y==20 | n==3 | x==2)
	{	
 		if (x==2)
 		{
 			_delay_ms(2000);
 		}
		return 0;
	}else
		return 1;
	
}

