// Pin sensore HC-SR04
const int trigPin = 4;
const int echoPin = 7;

// LED integrato
const int ledPin = 13;

// Parametri di misura
const unsigned long interval = 200;       // intervallo di misura in ms
const float tolerance = 2.0;              // soglia minima distanza in cm (rumore)
const float accelThreshold = 50.0;        // soglia accelerazione in cm/s²

// Variabili temporanee
unsigned long lastMeasurementTime = 0;
float lastDistance = 0;
float lastSpeed = 0;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  // LED spento all'avvio

  // Intestazioni per il Plotter Seriale
  Serial.println("Distanza_cm\tVelocita_cm_s\tAccelerazione_cm_s2");
}

float measureDistance() {
  // Pulizia segnale
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Impulso di trigger
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Lettura del tempo di risposta (eco)
  unsigned long duration = pulseIn(echoPin, HIGH, 30000); // timeout 30ms

  if (duration == 0) {
    return -1.0; // Errore di lettura
  }

  // Conversione in cm (velocità del suono ~343 m/s)
  float distanceCm = (duration / 2.0) * 0.0343;
  return distanceCm;
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastMeasurementTime >= interval) {
    lastMeasurementTime = currentTime;

    float currentDistance = measureDistance();

    if (currentDistance < 0) {
      // Oggetto non rilevato, spegne LED e stampa zeri
      digitalWrite(ledPin, LOW);
      Serial.println("0\t0\t0");
      lastDistance = 0;
      lastSpeed = 0;
    } else {
      float deltaD = currentDistance - lastDistance;
      float deltaT = interval / 1000.0; // in secondi

      float currentSpeed = abs(deltaD) > tolerance ? deltaD / deltaT : 0;
      float acceleration = (currentSpeed - lastSpeed) / deltaT;

      // Accendi il LED se l'accelerazione supera la soglia
      if (abs(acceleration) >= accelThreshold) {
        digitalWrite(ledPin, HIGH);
      } else {
        digitalWrite(ledPin, LOW);
      }

      // Output per il Plotter Seriale
      Serial.print(currentDistance, 2);
      Serial.print("\t");
      Serial.print(currentSpeed, 2);
      Serial.print("\t");
      Serial.println(acceleration, 2);

      // Aggiorna i valori
      lastDistance = currentDistance;
      lastSpeed = currentSpeed;
    }
  }
}
