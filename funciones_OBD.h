#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h> 
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* abs */
#include <math.h>       /* floor */


// defines for eeprom access
#define read_eeprom_byte(address) eeprom_read_byte ((const uint8_t*)address)
#define write_eeprom_byte(address,value) eeprom_write_byte ((uint8_t*)address,(uint8_t)value)
#define read_eeprom_word(address) eeprom_read_word ((const uint16_t*)address)
#define write_eeprom_word(address,value) eeprom_write_word ((uint16_t*)address,(uint16_t)value)
#define read_eeprom_dword(address) eeprom_read_dword ((const uint32_t*)address)
#define write_eeprom_dword(address,value) eeprom_write_dword ((uint32_t*)address,(uint32_t)value)
#define read_eeprom_array(address,value_p,length) eeprom_read_block ((void *)value_p, (const void *)address, length)
#define write_eeprom_array(address,value_p,length) eeprom_write_block ((const void *)value_p, (void *)address, length)


#define rpm_header "410C"
#define rpm_code "010C1\r"
#define rpm_len 8
#define rpms 0

#define speed_header "410D"
#define speed_code "010D1\r"
#define speed_len 6
#define speed 1

#define throttle_header "4111"
#define throttle_code "01111\r"
#define throttle_len 6
#define throttle 2

#define load_header "4104"
#define load_code "01041\r"
#define load_len 6
#define load 3


#define MAF_header "4110"
#define MAF_code "01101\r"
#define MAF_len 8
#define consumo 4

#define no_data "NO DATA"
#define conecting "SEARCHING..."

#define num_menus 1 
#define mem_control 190
#define max_memorias 3
#define tiempo_muestras 300     //Tiempo entre muestras en segundos
#define tiempo_entre_aceleraciones 1000 //en milisegundos 
#define tiempo_entre_cambios 100 //en milisegundos 


u8g_t u8g;

unsigned int marcado_rpms=0, rpms_max=0, revoluciones_guardadas=0, vel=0, vel_max=0, acel=1, carga=0, min_acel=99, max_acel=1, revols=0, cuenta_anterior=0, cuenta_cambios=0,
			 vel_ant=0;			 
long double tiempo_actual=0;
double distancia[3], cons=0, gasolina_consumida=0, consumo_medio=0, vel_med=0, rev_med=0, suma_revoluciones=0, variacion_tiempo=0, tiempo_anterior=0, tiempo_acel=0, aceleracion=0,
	   aceleracion_max=0, temp=0, tiempo_cambio=0;	   
unsigned char tiempo_marcado=10, byte_control=0, num_opciones=0, marcha_ant=0, marcha=0;
volatile char seleccion=0, opcion=0, ok=0;


void convert_values(char unsigned );
unsigned int hex2int(char [], char );
void USART_send( unsigned char );
void USART_putstring(char* );
int USARTReadChar();
char leer(char [],char [],char , unsigned int* );

