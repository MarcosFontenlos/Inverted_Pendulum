// Librerias I2C para controlar el mpu6050
// la libreria MPU6050 .h necesita I2Cdev .h, I2Cdev .h necesita Wire .h
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

 // La d i r e c c i n del MPU6050 puede ser 0 x68 o 0x69 , dependiendo
 // del estado de AD0 . Si no se especifica , 0 x68 e s t a r implicito
MPU6050 sensor ;

 // Valores RAW ( sin procesar ) del acelerometro y giroscopio en los ejes x,y,z
int16_t ax , ay , az ;
int16_t gx , gy , gz ;

void setup () {
  Serial . begin (57600) ; // Iniciando puerto serial
  Wire . begin () ; // Iniciando I2C
  sensor . initialize () ; // Iniciando el sensor

  if ( sensor . testConnection () ) Serial . println (" Sensor iniciado correctamente ");
  else Serial . println (" Error al iniciar el sensor ");
}

void loop () {
 // Leer las aceleraciones y velocidades angulares
  sensor . getAcceleration (& ax , & ay , & az ) ;
  sensor . getRotation (& gx , & gy , & gz );

 // Mostrar las lecturas separadas por un [tab]
  Serial . println ("a[x y z] g[x y z ]:\ t");
  Serial . print ( ax ); Serial . print ("\t");
  Serial . print ( ay ); Serial . print ("\t");
  Serial . println ( az ); Serial . print ("\t");
  Serial . print ( gx ); Serial . print ("\t");
  Serial . print ( gy ); Serial . print ("\t");
  Serial . println ( gz );

  delay (1000) ;
}