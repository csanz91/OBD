#define max_marcado_rpms 3500
#define duracion_marcado 10
#define limite_min_vel 10

void marcar_rpms(unsigned int* );
void detectar_rpms_max();
void detectar_vel_max();
void velocidad_media(double);
void calcular_min_throttle(unsigned int*, char);
void medir_consumo();
void calcular_distancia();
void calcular_rpms_medias();
void calcular_gasolina_consumida(double);
void calcular_consumo_medio();
void comprobar_datos(char[]);
void guardar_memoria();
void contar_tiempo();
void calcular_max_throttle(unsigned int*,char);
char detectar_marcha();
void cambio_marcha();
void calcular_aceleracion();
void calcular_temp(void);