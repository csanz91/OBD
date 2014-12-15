#include "funciones_OBD.h"
#include "funciones.c"
#include "UART.c"


unsigned int hex2int(char data[], char len)
{
	
	int unsigned suma=0;
	
	for (int n=4; n<len;n++)
	{
		if ((data[n]>='A') & (data[n]<='F'))
		{
			suma+=(data[n]-'A'+10)*pow(16,len-1-n);
		}
		else if ((data[n]>='0') & (data[n]<='9'))
		{
			suma+=(data[n]-'0')*pow(16,len-1-n);
		}
	}
	if (suma>15 || (suma==15 && data[len-1]!='F'))
	{
		suma++;
	}

	return suma;
}




char leer(char header[],char code[],char len, unsigned int* parametro)
{
	char data[10], correcto=0;
	
	USART_putstring(code);
	correcto=USARTReadString(data,len,header);
	
	if (correcto)
	{
		*parametro=0;
		return 0;
	}else
	{
		*parametro=hex2int(data,len);
		return 1;
	
	}
	
}


