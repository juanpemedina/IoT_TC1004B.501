/*
 *  This example shows how to use WPA2 enterprise
 *  Written by: Jeroen Beemster
 *  12 July 2017
 *  Version 1.00
 */

// Incluir Librerias para el ESP32, WiFi, Firebase
#include <esp_wpa2.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include "Arduino.h"
#include "addons/RTDBHelper.h"

// Conexion con el Sensor de Temperatura y Humedad DHT11
#include "DHT.h" // Incluye libreria del Sensor DHT11
#define DHTPIN 16 // Esta conectado al Pin 16
#define DHTTYPE DHT11   // DHT 11
// -----------------------------------------------------

// Conexion con Internet
const char* ssid = "Tec";
#define EAP_IDENTITY "a01732855@tec.mx"
#define EAP_PASSWORD "Tada$hi2003"
// -----------------------------------------------------

// Conexion con Sensor de Luz
#define LIGHT_SENSOR_PIN 36 // ESP32 pin GIOP36 (ADC0)
// -----------------------------------------------------

// Conexion con el Sensor de Distancia 
const int trigPin = 13; // Conectado al Pin 13
const int echoPin = 27; // Conectado al Pin 27
// -----------------------------------------------------

// Conexion con el Sensor de Presencia
#define SOUND_SPEED 0.034 
long duration;
float distanceCm;

const int PIN_TO_SENSOR = 19; // GIOP19 pin connected to OUTPUT pin of sensor Presencia
int pinStateCurrent   = LOW; 
// -----------------------------------------------------


// Insert Firebase project API Key - Llave de la Firebase del Reto
#define API_KEY "AIzaSyAzXLpt54jZP6Q3yUQoEYq2A-v_0vDQix8"//AIzaSyAzXLpt54jZP6Q3yUQoEYq2A-v_0vDQix8

// Insert RTDB URLefine the RTDB URL del Firebase del Reto */
#define DATABASE_URL "https://si-quieres-te-la-saco-default-rtdb.firebaseio.com/"

//Define Firebase Data object
FirebaseData fbdo;

// Definir configuracion del Firebase
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int intValue;
float floatValue;

bool signupOK = false;
int count = 0;
DHT dht(DHTPIN, DHTTYPE); // Inicializar funcion del Sensor DHT11

// A PARTIR DE AQUI SE INSERTA LA FUNCION PARA EL DISLPAY
//se demuestra el manejo del display de siete segmentos

//se declaran los pines a usar
int LEDs[] = {22,23,1,3,2,18,5};    // for ESP32 microcontroller

//se declaran los arreglos que forman los dígitos
//////////////A  B  C  D  E  F  G
int zero[] = {1, 1, 1, 1, 1, 1, 0};   // cero
int one[] = {0, 1, 1, 0, 0, 0, 0};   // uno
int two[] = {1, 1, 0, 1, 1, 0, 1};   // dos
int three[] = {1, 1, 1, 1, 0, 0, 1};   // tres
int four[] = {0, 1, 1, 0, 0, 1, 1};   // cuatro 
int five[] = {1, 0, 1, 1, 0, 1, 1};   // cinco
int six[] = {1, 0, 1, 1, 1, 1, 1};   // seis
int seven[] = {1, 1, 1, 0, 0, 0, 0};   // siete
int eight[] = {1, 1, 1, 1, 1, 1, 1}; // ocho
int nine[] = {1, 1, 1, 0, 0, 1, 1};   // nueve

int line[] = {0, 0, 0, 0, 0, 0, 1}; //Linea [-]

// Setup de los sensores, display y conexiones
void setup() {
    Serial.begin(115200);
    delay(10);
    Serial.println(F("DHTxx test!"));

   for (int i = 0; i<7; i++) pinMode(LEDs[i], OUTPUT); // Setup del Display de 7 Segmentos

    // Setup del Sensor de Distancia
   pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
   pinMode(echoPin, INPUT); // Sets the echoPin as an Input

   pinMode(PIN_TO_SENSOR, INPUT); // Setup del Sensor de Presencua
    
    dht.begin(); // Inicia el DHT11

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    // Setup de la Conexion al Internet  --------------------------------------------------
    // WPA2 enterprise magic starts here
    WiFi.disconnect(true); 
    WiFi.mode(WIFI_STA);   //init wifi mode
  //esp_wifi_set_mac(ESP_IF_WIFI_STA, &masterCustomMac[0]);
  Serial.print("MAC >> ");
  Serial.println(WiFi.macAddress());
  Serial.printf("Connecting to WiFi: %s ", ssid);
  //esp_wifi_sta_wpa2_ent_set_ca_cert((uint8_t *)incommon_ca, strlen(incommon_ca) + 1);
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  //esp_wpa2_config_t configW = WPA2_CONFIG_INIT_DEFAULT();
  //esp_wifi_sta_wpa2_ent_enable(&configW);
  esp_wifi_sta_wpa2_ent_enable();
    // WPA2 enterprise magic ends here


    WiFi.begin(ssid);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // Configuracion y Acceso a la Firebase del Reto ----------------------------------------
///* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}



// Ciclo infinito para lectura de datos de sensores e interaccion con el Display -------------------------------------
void loop() {
  int digito;

  // Datos del Sensor de Presencia
  pinStateCurrent = digitalRead(PIN_TO_SENSOR); 
  int presencia;
  if(pinStateCurrent == HIGH) {   
    presencia = 1;
  }
  else if(pinStateCurrent == LOW) { 
    presencia = 0;
  }
  // -------------------------------------------
  
  // Datos del Sensor de Distancia
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  // -------------------------------------------
  
  // Datos del Sensor de Temperatura y Humedad
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  // -------------------------------------------

  int sensorLuz = analogRead(LIGHT_SENSOR_PIN); // Datos del Sensor de Luz
  
  
    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

  

   ///////////// Mandar a la base de datos el valor del sensor de la Humedad
    if (Firebase.RTDB.setFloat(&fbdo, "test/humedad", h)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

 
    ///////// Mandar a base de datos el valor del sensor de temperatura en Celsius 
    if (Firebase.RTDB.setFloat(&fbdo, "test/TemperaturaC",hic )){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    
    ////////////// Manda a base de datos el valor del sensor de temperatura en Fahrenheit
    if (Firebase.RTDB.setFloat(&fbdo, "test/TemperaturaF", hif)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    
    ///////////////// Mandar a base de datos el valor del sensor de luz

    if (Firebase.RTDB.setFloat(&fbdo, "test/Luz", sensorLuz)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    ///////////////// Mandar a base de datos el valor del sensor de distancia

    if (Firebase.RTDB.setFloat(&fbdo, "test/Distancia", distanceCm)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    ///////////////// Mandar a base de datos el valor del sensor de presencia
    
    if (Firebase.RTDB.setFloat(&fbdo, "test/Presencia", presencia)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }

  // Lectura del valor de Digito en la Base de Datos del Firebase
  String num;
  if(Firebase.RTDB.getString(&fbdo, "test/digito")){ // Al encontrarse el elemento deseado 
    num = fbdo.stringData();
    digito = num.toInt(); // Se agrega el numero hallado a la variable 
    Serial.println("Digito: ");
    Serial.println(digito);
  }
  switch(digito) // Para mandar dicho numero al Display
    {
        case 0:
                    for (int i = 0; i<7; i++) digitalWrite(LEDs[i], zero[i]);
                    break;
        case 1:
                    for (int i = 0; i<7; i++) digitalWrite(LEDs[i], one[i]);
                    break;
        case 2:
                    for (int i = 0; i<7; i++) digitalWrite(LEDs[i], two[i]);
                    break;
        case 3:
                    for (int i = 0; i<7; i++) digitalWrite(LEDs[i], three[i]);
                    break;
        case 4:
                    for (int i = 0; i<7; i++) digitalWrite(LEDs[i], four[i]);
                    break;
        case 5:
                    for (int i = 0; i<7; i++) digitalWrite(LEDs[i], five[i]);
                    break;
        case 6:
                    for (int i = 0; i<7; i++) digitalWrite(LEDs[i], six[i]);
                    break;
        case 7:
                    for (int i = 0; i<7; i++) digitalWrite(LEDs[i], seven[i]);
                    break;
        case 8:
                    for (int i = 0; i<7; i++) digitalWrite(LEDs[i], eight[i]);
                    break;
        case 9:
                    for (int i = 0; i<7; i++) digitalWrite(LEDs[i], nine[i]);
                    break;
        default:
                    for (int i = 0; i<7; i++) digitalWrite(LEDs[i], line[i]);
                    break;
              
    }

} // FIN DEL PROGRAMA
