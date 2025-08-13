int PWMA =5;      // Velocidad motores derecha
int AIN1 =7;     // Dirección motores derecha
int STBY =3;      // Stand-by del puente
int VELOCIDAD =220;   // Velocidad de ensayo (0-255)
#include <Servo.h> // encargada de la abstracción de la Modulación por ancho de pulsos.  
Servo miServoA; // Objeto encargado de administrar todas las órdenes a través de él
Servo miServoB; //Mueve al motor ultrasónico
//Servo panServo;// pin 10
//Servo dirServo; //pin 11
int TRIG_PIN = 13;     // Pin que envía el pulso
int ECHO_PIN = 12;     // Pin que recibe el eco
long TIMEOUT_US = 30000UL; // 30 ms → ~5 m
/* Ángulos de barrido del sensor (0 170°) */
const int MIN_PAN = 0;
const int MAX_PAN = 170;
const int STEP_PAN = 20;
/* ---------- Constantes de control ---------- */
int SPEED_FWD  = 190;   // 0 255
const int SPEED_REV  = 160;   // retroceso lento
const unsigned int SAFE_DIST = 30;   // cm
const unsigned long SCAN_DELAY = 120;  // milisegundos entre mediciones
/* Ángulos de dirección (ajusta a tu mecánica) */
const int CENTER_DIR = 90;
const int LEFT_DIR   = 150;
const int RIGHT_DIR  = 30;





void setup() {
 
   miServoA.attach(11); // Pin 10 o Pin 11.
   miServoB.attach(10); // Mueve al motor ultrasónico
   Serial.begin(9600);
   pinMode(PWMA, OUTPUT);
   pinMode(AIN1, OUTPUT);
   pinMode(STBY, OUTPUT);
   digitalWrite(STBY, HIGH);  // Activa el TB6612
   miServoA.write(90);
   miServoB.write(90);
   Serial.println("===== Test de motores iniciado =====");
   pinMode(TRIG_PIN, OUTPUT);
   pinMode(ECHO_PIN, INPUT);
   digitalWrite(TRIG_PIN, LOW);          // Asegura nivel bajo inicial
   Serial.println("HC-SR04 listo…");
}

void loop() {
  /* 1. Avance normal */
  miServoB.write(90);
  atrasMD(SPEED_FWD);
  miServoA.write(90); // mirada frontal
  unsigned int d = readDistance();
    if (d < SAFE_DIST) {
    /* 2. Obstáculo detectado */
    detener();
    Serial.print(F("Obstáculo a "));
    Serial.print(d);
    Serial.println(F(" cm → Escanear..."));

        /* 3. Barrido panorámico */
    int bestAngle = scanBestAngle();
    Serial.print(F("Mejor ángulo: "));
    Serial.println(bestAngle);

        /* 4. Toma de decisión */
    if (bestAngle < 70) {                 // zona izquierda
      Serial.println(F("Giro a la derecha"));
      miServoB.write(RIGHT_DIR);
      delay(300);
      atrasMD(SPEED_FWD);
      delay(500);
    }
       if (bestAngle > 110) {                 // zona derecha
      Serial.println(F("Giro a la izquierda"));
      miServoB.write(LEFT_DIR);
      delay(300);
      atrasMD(SPEED_FWD);
      delay(500);
    }
        else {
      /* No hay salida clara: retroceder */
      Serial.println(F("Sin salida → Retroceso"));
      adelanteMD(SPEED_REV);
      delay(2
      00); //originalmente estaba a 500
    }
   
    /* 5. Reajustar */
    detener();
    centerAll();
  }

  delay(50); // bucle suave

    }
   
   

 
// Función  de control Adelante motores del lado derecho
void adelanteMD(int VELOCIDAD)  
{
 analogWrite(PWMA, VELOCIDAD);
 digitalWrite(AIN1, HIGH);
}
// Función de control  Atras motores del lado derecho
void atrasMD(int SPEED_FWD)  
{
 analogWrite(PWMA, VELOCIDAD);
 digitalWrite(AIN1, LOW);
}

void detener() {
  analogWrite(PWMA, 0);
  digitalWrite(STBY, LOW);      // Desactiva brevemente el puente
  delay(10);
  digitalWrite(STBY, HIGH);
  Serial.println(F("Motores detenidos"));
}

float medirDistanciaCm(int trigPin, int echoPin) {
  /* 1. Pulso de disparo: 10 µs en alto */
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  /* 2. Duración del eco con control de tiempo máximo */
  long dur = pulseIn(echoPin, HIGH, TIMEOUT_US);
  if (dur == 0) return -1.0;            // Sin eco → error/rango excedido
  /* 3. Conversión a cm (velocidad del sonido ~343 m/s a 20 °C) */
  float distancia = dur / 58.0;         // 58 µs ≈ 1 cm
  /* 4. Verificación de límites prácticos */
  if (distancia < 2 || distancia > 400) return -1.0;
  return distancia;
}

/* ---------- Lectura de distancia ---------- */
unsigned int readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000UL); // 30 ms ⇒ ~5 m
  if (duration == 0) return 500; // fuera de rango

  return duration / 58; // μs → cm
}


int scanBestAngle() {
  unsigned int bestDist = 0;
  int bestAngle = 90;

  for (int angle = MIN_PAN; angle <= MAX_PAN; angle += STEP_PAN) {
    miServoA.write(angle);
    delay(150); // tiempo para que el servo llegue y el eco se estabilice
    unsigned int d = readDistance();
    if (d > bestDist) {
      bestDist = d;
      bestAngle = angle;
    }
    delay(SCAN_DELAY);
  }
  return bestAngle; // devuelve el ángulo con más espacio
}
/* ---------- Funciones auxiliares ---------- */
void centerAll() {
  miServoA.write(90);
  miServoB.write(CENTER_DIR);
}
