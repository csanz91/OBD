/*
 * OBD.c
 *
 * Created: 11/05/2013 16:10:38
 *  Author: Cesar
 */ 
#include "u8g.h"
#include "funciones_OBD.c"


void u8g_setup(void)
{  
  /*
	K (backlight cathode)  GND
	A (backlight annode)   +5V
	PSD (SPI Mode)          GND (SEE NOTE)
	E (SCK)                PORTB, Pin 5
	R/W (MOSI)             PORTB, Pin 3
	RS (CS)                PORTB, Pin 4
	VDD                    +5V
	VSS                    GND
  */
  u8g_InitHWSPI(&u8g, &u8g_dev_st7920_128x64_4x_hw_spi, PN(1,4), U8G_PIN_NONE, U8G_PIN_NONE);
  
  /* flip screen, if required */
  u8g_SetRot180(&u8g);

  /* assign default color value */
  if ( u8g_GetMode(&u8g) == U8G_MODE_R3G3B2 ) 
    u8g_SetColorIndex(&u8g, 255);     /* white */
  else if ( u8g_GetMode(&u8g) == U8G_MODE_GRAY2BIT )
    u8g_SetColorIndex(&u8g, 3);         /* max intensity */
  else if ( u8g_GetMode(&u8g) == U8G_MODE_BW )
    u8g_SetColorIndex(&u8g, 1);         /* pixel on */
}


void sys_init(void)
{
	#if defined(__AVR__)
	/* select minimal prescaler (max system speed) */
	CLKPR = 0x80;
	CLKPR = 0x00;
	#endif
}

int main(void)
{
	//Configuracion del temporizador
	TCCR1B |= ((1 << CS10) | (0 << CS11) | (1 << CS12)); // Start timer at Fcpu/1024
		
	//Configuracion interrupciones
	PCICR |= (1 << PCIE1);  //Pin change interrupt 1. PCINT8..14
	PCMSK1|= (1 << PCINT13) | (1 << PCINT12) | (1 << PCINT11) | (1 << PCINT10) | (1 << PCINT9);
			
	//Configuracion puertos
	PORTC|=(1<<PINC5);//(1<<PINC1) | (1<<PINC2) | (1<<PINC3) | (1<<PINC4) | 
	
	//Configuracion ADC
	ADMUX|=(1<<REFS0) |(1<<REFS1);  //Referencia interna
	ADMUX &= ~(1<<MUX0) & ~(1<<MUX1) & ~(1<<MUX2) & ~(1<<MUX3);  //Seleccionamos ADC0
	ADCSRA |= (0<<ADPS0) | (1<<ADPS1) | (1<<ADPS2); //Factor de 64
	ADCSRA|=(1<<ADEN);

	
	//Inicializacion USART
	UCSR0B=(1<<RXEN0)|(1<<TXEN0);
	UCSR0C=(3<<UCSZ00);
	UCSR0A|=(1<<U2X0);
	
	UBRR0L = 25;
	UBRR0H = (25>>8);
	
	//Inicializacion Elm327


	USART_putstring("ATSP0\r"); //Selecciona automaticamente el protocolo
	_delay_ms(3000);
	USART_putstring("ATE0\r"); //Desactiva el eco de los comandos enviados
	_delay_ms(500);
	USART_putstring("ATS0\r");  //Quita los espacios entre cada caracter
	_delay_ms(500);
 	USART_putstring("ATAT2\r"); //Max velocidad		
	 _delay_ms(500);
 	USART_putstring("ATL0\r"); //Desactivar marcas
		_delay_ms(500);
		
	//Inicializacion LCD
	sys_init();
	u8g_setup();	
	
	//Enable Global Interrupts
	sei();
	
    while(1)
    {
		contar_tiempo();
		calcular_rpms_medias();
		calcular_distancia();
		calcular_consumo_medio();
		calcular_temp();
		//USART_putstring("ACABAMOS/r");
	
		u8g_FirstPage(&u8g);
		do
		{
			draw();
		} while ( u8g_NextPage(&u8g) );
		//USART_putstring("EMPEZAMOS/r");
	}    
}

ISR(PCINT1_vect)
{	
	if (!bit_is_set(PINC,PINC5))
	{
		if (seleccion==0)
		{
			seleccion=num_menus;
		}else
		{
			seleccion--;
		}
	}
		
	}else if (bit_is_set(PINC,PINC1))
		{
			ok=!ok;
		}
	
	else if (bit_is_set(PINC,PINC4))
	{
		if (seleccion==num_menus)
		{
			seleccion=0;
		}else
		{
			seleccion++;
		}
	}else if (bit_is_set(PINC,PINC3))
	{
		if (opcion==num_opciones)
		{
			opcion=0;
		}else
		{
			opcion++;
		}
	}else if (bit_is_set(PINC,PINC2))
	{
		if (opcion==0)
		{
			opcion=num_opciones;
		}else
		{
			opcion--;
		}

	}
	_delay_ms(50);

	PCIFR |= (1 << PCIF1); 
}



