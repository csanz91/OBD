#include "funciones.h"


void detectar_rpms_max()
{
	if (revols>rpms_max)
	{
		rpms_max=revols;
	}		
}

void detectar_vel_max()
{
	if (vel>vel_max)
	{
		vel_max=vel;
	}
}

void marcar_rpms(unsigned int* revoluciones)
{
	if (marcado_rpms)
	{
		if (*revoluciones>max_marcado_rpms)
		{
			revoluciones_guardadas=*revoluciones;
			tiempo_marcado=0;
		}else if ( tiempo_marcado<duracion_marcado)
		{
			*revoluciones=revoluciones_guardadas;
			tiempo_marcado=tiempo_marcado+1;
		}
					
	}
}

void velocidad_media(double tiempo)
{
	if (distancia[2]>500)
	{
		vel_med=(distancia[2]*360)/tiempo;
	}
	
}

void calcular_min_throttle(unsigned int* min_throttle, char correcto)
{
	if ((acel<*min_throttle) & (correcto))
	{
		*min_throttle=acel;
	}	
}

void calcular_max_throttle(unsigned int* max_throttle, char correcto)
{
	if ((acel>*max_throttle) & (correcto))
	{
		*max_throttle=acel;
	}
}

void medir_consumo()
{
	unsigned int valor_MAF;
	char correcto=0;	
	correcto=leer(throttle_header,throttle_code,throttle_len, &acel);
	calcular_min_throttle(&min_acel, correcto);
	calcular_max_throttle(&max_acel, correcto);	
	if (correcto)
	acel=(100.0/(max_acel-min_acel))*(acel-min_acel);
	correcto=(leer(MAF_header,MAF_code,MAF_len, &valor_MAF) & correcto);
	correcto=(leer(load_header,load_code,load_len, &carga) & correcto);

	if ((acel<4) & (revols>1500)) 
	{
		valor_MAF=0;
	}

	if (correcto)
	{			
		if ((vel<limite_min_vel) | (revols<1000))
		{
			//cons = 0.3021496333*valor_MAF;
			cons = 0.4021496333*valor_MAF;
			
		}else	
		{
			cons= (0.4021496333*valor_MAF)/(vel);

		}
		
		if (!revols)
		{
			cons=0;
		}
		
		if (cons>99.9)
		{
			cons=99.9;
		}
		
	}else
	{
		cons=0;
	}

}

void calcular_distancia()
{
	
	long double aux=0, variacion_distancia=0;
	aux=tiempo_actual;
	variacion_tiempo=aux-tiempo_anterior;
	tiempo_anterior=aux;
	
	if (leer(speed_header, speed_code,speed_len, &vel))
	{

		variacion_distancia=(variacion_tiempo*vel*10)/3600;  //Distancia en m
		
		for (int n=1;n<3;n++)   //Suma la distancia a todos los cuentakilometros
		{
			distancia[n]=distancia[n]+variacion_distancia;
		}
		
		calcular_gasolina_consumida(variacion_distancia);
		velocidad_media(aux);
		cambio_marcha();
		detectar_vel_max();
		
	}else
	{
		acel=0;
		carga=0;
		cons=0;
		
	}
}

void calcular_rpms_medias()
{
	char correcto;
	correcto=leer(rpm_header,rpm_code,rpm_len, &revols);
	revols=revols/4;
	
	if ((revols>0) & correcto)
	{
		suma_revoluciones=suma_revoluciones+((revols*variacion_tiempo)/6000);
		rev_med=(suma_revoluciones*6000)/tiempo_anterior;
		detectar_rpms_max();
	}
			
}

void calcular_gasolina_consumida(double variacion_distancia)   //La llamamos desde calcular_distancia
{	
	medir_consumo();	

	gasolina_consumida=gasolina_consumida+(cons*variacion_distancia*0.01);   //Resultado en mililitros
	
}

void calcular_consumo_medio()
{
	// the car has not moved yet or no fuel has been used
	if((distancia[2]<500) | (gasolina_consumida==0))
	{
		// will display 0.00L/100 or 999.9mpg
		consumo_medio=0;
	}
	else  // the car has moved and fuel used
	{
		consumo_medio=(gasolina_consumida)/(distancia[2])*100; // div by 0 avoided by previous test

		if(consumo_medio>99.99)    // SI
		consumo_medio=99.99;     // display 99.99 L/100 maximum
	}		
		
}


void draw(void)
{
	
	if (!seleccion)
	{
		char cad_vel[10], cad_temp[10], cad_cons[10], cad_revs[10], cad_tiempo[10];
		int aux;
		
		//Generamos las cadenas
		sprintf(cad_vel, "Vel:%3u", vel);
		sprintf(cad_revs, "Revs:%4u", revols);
		sprintf(cad_cons, "Cons:%2.1f, Med%2.1f", cons, consumo_medio);
		sprintf(cad_temp, "Temp:%2.1f", temp);
		//sprintf(cad_tiempo, "%.0f", tiempo_actual/6000.0);
		
		//Dibujamos las cadenas
		u8g_SetFont(&u8g, u8g_font_8x13);
		u8g_DrawStr(&u8g, 0, 10, cad_vel);
		u8g_DrawStr(&u8g, 0, 27, cad_revs);
		u8g_DrawStr(&u8g, 0, 44, cad_cons);
		u8g_DrawStr(&u8g, 0, 61, cad_temp);
		//u8g_DrawStr(&u8g, 97, 61, cad_tiempo);
		//u8g_SetFont(&u8g, u8g_font_4x6);
		//u8g_DrawStr(&u8g, 77, 61, "Tmpo");
		//u8g_DrawStr(&u8g, 108, 7, "pedal");
		//u8g_DrawFrame(&u8g,85,10,13,35);
		//u8g_DrawFrame(&u8g,110,10,13,35);
		//aux=floor((acel*35)/100.0);
		//u8g_DrawBox(&u8g,110,45-aux,13,aux+1);
		
		//if (!ok)
		//{
			//u8g_DrawStr(&u8g, 82, 7, "carga");
			//aux=floor((carga*35)/255.0);
			//u8g_DrawBox(&u8g,85,45-aux,13,aux+1);
		//}else
		//{
			//u8g_DrawStr(&u8g, 84, 7, "acel");
			//aux=floor((aceleracion*35.0)/aceleracion_max);
			//u8g_DrawBox(&u8g,85,45-aux,13,aux+1);
		//}
		
		
	}else if (seleccion)
	{
		char cad_vel_max[10], cad_vel_med[10], cad_revs_max[10], cad_revs_med[10], cad_cuenta_cambios[10], cad_aceleracion[10];
		//Generamos las cadenas
		sprintf(cad_vel_max, "%03u", vel_max);
		sprintf(cad_revs_max, "%04u", rpms_max);
		sprintf(cad_vel_med, "%03.0f", vel_med);
		sprintf(cad_revs_med, "%04.0f", rev_med);
		sprintf(cad_cuenta_cambios, "%04u", cuenta_cambios);
		sprintf(cad_aceleracion, "%02.2f", aceleracion_max);
		
		//Dibujamos las cadenas
		u8g_SetFont(&u8g, u8g_font_7x13);
		u8g_DrawStr(&u8g, 0, 20, "Velocidad:");
		u8g_DrawStr(&u8g, 0, 33, "Revolucns:");
		u8g_DrawStr(&u8g, 0, 46, "Nºcambios:");
		u8g_DrawStr(&u8g, 0, 60, "Acelrcion:");
		u8g_SetFont(&u8g, u8g_font_6x10);
		u8g_DrawStr(&u8g, 0, 8, "Resumen");
		u8g_DrawFrame(&u8g,76,0,24,11);
		u8g_DrawStr(&u8g, 79, 9, "MAX");
		u8g_DrawFrame(&u8g,102,0,24,11);
		u8g_DrawStr(&u8g, 105, 9, "MED");
		u8g_DrawStr(&u8g, 77, 20, cad_vel_max);
		u8g_DrawStr(&u8g, 103, 20, cad_vel_med);
		u8g_DrawStr(&u8g, 77, 33, cad_revs_max);
		u8g_DrawStr(&u8g, 103, 33, cad_revs_med);
		u8g_DrawStr(&u8g, 77, 46, cad_cuenta_cambios);	
		u8g_DrawStr(&u8g, 77, 60, cad_aceleracion);	
		
	}
	
}

void contar_tiempo()
{

	unsigned int aux=0, variacion=0;
	
 	if (revols>0)
 	{
		aux=TCNT1;
		if (aux<cuenta_anterior)
		{
			variacion=aux;
			variacion=variacion+(65535-cuenta_anterior);
		}else
		{
			
			variacion=aux-cuenta_anterior;
		}
		tiempo_actual=tiempo_actual+variacion*0.012911;
		cuenta_anterior=aux;
	}
}


char detectar_marcha()
{
	float relacion;
	
	if (vel)
	{
		relacion=revols/vel;
		
		if ((relacion>110) & (relacion<200))
		{
			return 1;
		}
		else if ((relacion>65) & (relacion<100))
		{
			return 2;
		}
		else if ((relacion>42) & (relacion<60))
		{
			return 3;
		}
		else if ((relacion>34) & (relacion<41))
		{
			return 4;
		}
		else if ((relacion>25) & (relacion<31))
		{
			return 5;
		}
		else
		{
			return 0;
		}
	}else
	{
		return 0;
	}
}

void cambio_marcha()
{
	marcha=detectar_marcha();
	
	if ((marcha!=marcha_ant) & (marcha!=0) & ((tiempo_actual-tiempo_cambio)>tiempo_entre_cambios))
	{
		cuenta_cambios++;
		marcha_ant=marcha;
		tiempo_cambio=tiempo_actual;
	}
}

void calcular_temp(void)
{
	double valores=0;
	
	for(int x=0;x<200;x++)
	{
		ADCSRA|=(1<<ADSC);
		while (ADCSRA & (1<<ADSC) );
		valores=valores+(ADCW*0.0010752688)/(0.01);
	}
	temp=valores/200;
}

void calcular_aceleracion()
{
	double diferencia_tiempos=0;
	
	diferencia_tiempos=tiempo_actual-tiempo_acel;
	
	if (diferencia_tiempos>tiempo_entre_aceleraciones)
	{
		aceleracion=(abs(vel_ant-vel))/(diferencia_tiempos/1000.0); //Aceleracion en km/h por segundo
		vel_ant=vel;
		tiempo_acel=tiempo_actual;	
	}
	
	//Comprobando si la aceleracion es maxima
	if (aceleracion>aceleracion_max)
	{
		aceleracion_max=aceleracion;
	}
}
