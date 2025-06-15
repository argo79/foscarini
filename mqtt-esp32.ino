#include <WiFi.h>
#include <PubSubClient.h>

// Configurazione Wi-Fi
const char* ssid = "RaspAP";
const char* password = "ChangeMe";

// Configurazione Broker MQTT
const char* mqtt_server = "192.168.50.1";  // esempio: "mqtt.example.com"
const int mqtt_port = 1883;  // Porta di default per MQTT (1883)

WiFiClient espClient;
PubSubClient client(espClient);

// Nome del dispositivo, che sarà la prima parte del topic
const char* deviceName = "fosca2";  //     <---------------------------Cambia questo valore per ogni dispositivo

// Costruzione dei topic MQTT
String ledTopic = String(deviceName) + "/lab/led";
String tempTopic = String(deviceName) + "/lab/temp";

// Pin per il LED
const int ledPin = 5;  // Pin D5

// Funzione per connettersi alla rete Wi-Fi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connessione alla rete WiFi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Connesso alla rete WiFi!");
  Serial.print("Indirizzo IP: ");
  Serial.println(WiFi.localIP());
}

// Funzione di callback per quando arriva un messaggio su un canale MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  // Controlla se il messaggio è su 'ledTopic'
  if (String(topic) == ledTopic) {
    Serial.print("Messaggio ricevuto su '");
    Serial.print(ledTopic);
    Serial.print("': ");
    Serial.println(message);
    
    // Accendi o spegni il LED in base al messaggio ricevuto
    if (message == "1") {
      digitalWrite(ledPin, HIGH);  // Accende il LED
      Serial.println("LED acceso");
    } else if (message == "0") {
      digitalWrite(ledPin, LOW);   // Spegne il LED
      Serial.println("LED spento");
    }
  }
}

// Funzione per connettersi al broker MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Connessione al broker MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("connesso!");
      client.subscribe(ledTopic.c_str());  // Iscriviti al topic dinamico
    } else {
      Serial.print("fallito, rc=");
      Serial.print(client.state());
      Serial.println(" riprovo in 5 secondi...");
      delay(5000);
    }
  }
}

// Funzione per generare un valore casuale tra 15.0 e 35.0
float generate_random_temp() {
  return random(150, 351) / 10.0; // Restituisce un valore tra 15.0 e 35.0
}

void setup() {
  // Inizializza la comunicazione seriale
  Serial.begin(115200);

  // Imposta il pin del LED come output
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Assicurati che il LED sia spento all'inizio
  
  // Connetti il dispositivo alla rete Wi-Fi
  setup_wifi();

  // Configura il client MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  // Se non siamo connessi al broker MQTT, riconnettiamo
  if (!client.connected()) {
    reconnect();
  }
  
  // Gestisci la connessione MQTT
  client.loop();

  // Ogni 30 secondi pubblichiamo un valore casuale su 'tempTopic'
  static unsigned long lastPublish = 0;
  if (millis() - lastPublish > 30000) {  // 30 secondi
    lastPublish = millis();
    
    float tempValue = generate_random_temp();
    char tempStr[8];
    dtostrf(tempValue, 1, 1, tempStr);  // Converte il valore float in stringa
    
    // Pubblica il valore sul topic 'tempTopic'
    Serial.print("Pubblico su '");
    Serial.print(tempTopic);
    Serial.print("': ");
    Serial.println(tempStr);
    client.publish(tempTopic.c_str(), tempStr);
  }
}
