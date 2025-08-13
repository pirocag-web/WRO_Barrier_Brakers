#include <Servo.h>
Servo miServoA; 
long tPrev;// marca de tiempo
int pos=30; // posición de inicio para el servo
int dir=1; // dirección +1 para avanzar y -1 para retroceder.
void setup() {
  miServoA.attach(11); // Pin 10 o Pin 11.
  miServoA.write(pos);
  tPrev = millis(); // tiempo actual en milisegundo
}


void loop() {
   if(millis() - tPrev >= 10){    // condición  para saber si ha pasado el tiempo indicado
    pos += dir; // asignación de dirección 
    miServoA.write(pos); // indicar al servo el ángulo de movimiento
    if(pos >= 170 || pos <= 10) dir = -dir;// limite del rango de movimiento
    tPrev = millis();// ajuste del tiempo
  }
}
