 /*
   Title: nodo iot
   brief: *
   rev: 1.51
   nota: -
   author: Luca Agostini
   */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266HTTPClient.h>
#include <DallasTemperature.h>
#include <defines.h>
#include <prototypes.h>
#include <Arduino_JSON.h>

SoftwareSerial SerialAT(13, 15); // RX, TX
OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

//MQTT login
const char* broker = "190.104.223.227";
const uint16_t brokerPort = 27001;

const char MQTT_clientID[] = SENSOR_NAME;
const char MQTT_user[] = "iot";
const char MQTT_passworld[] = "iotiot";

//WIFI login
const char* ssid = "IOT";
const char* password = "T3mp@tUr@...";
const char *code_ver = "1.5";

//Topicos
const char* mainTopic = SENSOR_AREA"/TemperaturaVia/" SENSOR_NAME "/MainData";
const char* topicSleepTime = SENSOR_AREA"/TemperaturaVia/" SENSOR_NAME "/Sleeptime";
const char* topicBootMode = SENSOR_AREA"/TemperaturaVia/" SENSOR_NAME "/BootMode";


WiFiClient WIFI_client;
PubSubClient mqtt;
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

//#ifndef USE_PANEL
//  ADC_MODE(ADC_VCC);
//#endif

//Variables globales . son configurables desde el programa.
long lastReconnectAttempt = 0;
long timer1 = 0;
uint16_t sleepTime = 10; //minuntos
uint16_t bootMode = 1;

template<typename T>
void mqttPublish(const char* topic, T data)
{
  const int memSize = 200;

  char mem[memSize];
  String(data).toCharArray(mem, memSize);
  mqtt.publish(topic, mem);
}

connectionType currentConnectionType = _wifi;

void setup() {

  EEPROM.begin(4096);
  initializeSerial();
  initializeNetwork(currentConnectionType);
  setSleepTime();
  initializeMQTT();
  //initializePins();

  if (mqttConnect()) {
    suscribeMQTT();
  }

  httpUpdater.setup(&httpServer);
  httpServer.begin();

  sensors.begin();
 

  timer1 = millis();

}

void suscribeMQTT()
{
  mqtt.subscribe(topicSleepTime);
  mqtt.subscribe(topicBootMode);
}

void setSleepTime()
{
  charByte eepromData;

  for (uint8_t i = 0; i < DATA_LENGTH ; i++)
  {
    eepromData.byteData[i] = EEPROM.read(EEPROM_ADDR_SLEEPTIME + i);
  }

  if (eepromData.uint16Data >= 1 && eepromData.uint16Data <= SLEEP_TIME_MAX)
  {
    sleepTime = eepromData.uint16Data;
  }

  Serial.print("\nTiempo de sleep configurado: ");
  Serial.print(sleepTime);
  Serial.print(" minutos\n");

}

void initializeSerial()
{
  // Set console baud rate
  Serial.begin(115200);
  delay(100);

  // Set GSM module baud rate
  SerialAT.begin(115200);
  delay(100);
}

void initializeNetwork(connectionType currentConnectionType)
{
  mqtt.setClient(WIFI_client);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  Serial.println();

  if ( currentConnectionType == _wifi )
  {
    for (unsigned long start = millis(); millis() - start < CONNECTION_TIMEOUT; )
    {
      Serial.print(".");
      if (WiFi.status() == WL_CONNECTED)
      {
        Serial.print("\nConnection to WIFI success\n");
        Serial.print("Local IP: ");
        Serial.print(WiFi.localIP());
        return;
      }
      delay(500);
    }
    turnOffSystem();
  
  }
  else if ( currentConnectionType == _default )
  {
    for (unsigned long start = millis(); millis() - start < CONNECTION_TIMEOUT; )
    {
      Serial.print(".");
      if (WiFi.status() == WL_CONNECTED)
      {
        Serial.print("\nConnection success\n");
      }
      delay(500);
    }
    Serial.print("\nFailed to connect through wifi, trying to connect with gsm..\n");
    //connectThroughGSM();

  }

}

void initializeMQTT()
{
  // MQTT Broker setup
  mqtt.setServer(broker, brokerPort);
  mqtt.setCallback(mqttCallback);
}

bool mqttConnect()
{
  Serial.print("Connecting to ");
  Serial.print(broker);

  if (!mqtt.connect(MQTT_clientID, MQTT_user, MQTT_passworld))
  {
    Serial.println(" MQTT connection failed\n");

  }
  Serial.println(" OK");

  return mqtt.connected();
}

void checkMessages()
{
  if (mqtt.connected()) {
    mqtt.loop();
  }
  else {
    //mqttConnect();
  }

}

/*
   brief: Maquina de estados principal
*/
void execProgram()
{
  switch (bootMode)
  {
    case BOOT_MODE_NORMAL:
      execNormalMode();
      break;
    case BOOT_MODE_SERVER:
      execServerMode();
      break;
    default:
      execNormalMode();
      break;
  }
}

/*
   brief: Imp. de maquinas de estados
*/
void loop()
{
  checkMessages();
  execProgram();
}

/*
   brief: El programa se pone a la escucha de conexiones entrantes.
          Establece una conexion tcp/ip http
*/
void execServerMode()
{
  httpServer.handleClient();

  if (timer1 + BOOT_SERVER_TIMEOUT <= millis()) //si paso determinado tiempo, que se duerma
  {
    turnOffSystem();
  }
}

/*
   brief: El programa funciona en modo normal, se debe implementar:
 *        * Sensado de informacion externa
 *        * Pull de perisfericos (ej. : botones, pantalla etc.)
 *        * Publicacion de topicos (MQTT)
*/
void execNormalMode()
{
  //TODO: sensar botones, mostrar datos, etc.

  if (timer1 + BOOT_NORMAL_TIMEOUT <= millis()) //si paso determinado tiempo, que se duerma
  {
    if (mqtt.connected()) 
    {
      publishTopics();
    }

    turnOffSystem();
  }
}

/*
   brief: Ingreso a modo ahorro de energia
*/
void turnOffSystem()
{  
#ifdef USE_PANEL
  Serial.print("\nTurning ESP to sleep mode for");
  Serial.print(sleepTime);
  Serial.print("minutes\n");
  ESP.deepSleep(sleepTime * 60e6);
#else
  Serial.print("\nBooting system..\n");
  ESP.restart();
#endif
}

/*
   brief: Callback de mensajes externos.
   nota: Prot. MQTT
*/
void mqttCallback(char* topic, byte * payload, unsigned int len)
{
  Serial.print("\nMessage arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.write(payload, len);
  Serial.println();

  //TOPICO SLEEP
  if (strcmp(topicSleepTime, topic) == 0)
  {
    charByte data;

    Serial.print("\nTopico sleep time");

    for (uint8_t i = 0; i < len; i++)
    {
      if (*(payload + i) <= ANCII_0 && *(payload + i) >= ANCII_9) {
        Serial.print("Sintaxis de mensaje incorrecta (deben ser caracteres numericos)\n");
        return;
      }
    }

    data.uint16Data = atoi((char*)payload);

    if (data.uint16Data == sleepTime) {
      Serial.print("\nDato repetido\n");
    }
    else if (data.uint16Data > SLEEP_TIME_MAX) {
      Serial.print("\nDato (");
      Serial.print(data.uint16Data);
      Serial.print(") demaciado grande\n");
    }
    else
    {
      sleepTime = data.uint16Data;

      for (uint8_t i = 0; i < DATA_LENGTH; i++) {
        EEPROM.write(EEPROM_ADDR_SLEEPTIME + i, data.byteData[i]);
      }

      if (EEPROM.commit()) {
        Serial.print("\nDato ");
        Serial.print(data.uint16Data);
        Serial.print(" almacenado en EEPROM correctamente\n");
      }
    }

  }
  else if (strcmp(topicBootMode, topic) == 0)
  {
    if ( atoi((char*)payload) == BOOT_MODE_SERVER )
    {
      Serial.print("Boot mode Server\n");
      bootMode = BOOT_MODE_SERVER;
    }
    else
    {
      Serial.print("Boot mode Normal\n");
      bootMode = BOOT_MODE_NORMAL;
    }

    timer1 = millis();

  }

  delay(500);
  return;
}

/*
   brief: Publicacion de topicos
   nota: Implementa protocolo MQTT
*/
void publishTopics()
{

  JSONVar mqttMessage;
  JSONVar payload;

#ifdef USE_PANEL
  float voltage = analogRead(0)*4.101e-3;
#else
  float voltage = analogRead(0)*4.897e-3;
#endif
  Serial.print("Publish topics..\n");

  sensors.requestTemperatures();
  //mlx.readObjectTempC();
  //mlx.readAmbientTempC();

  payload["data"] = sensors.getTempCByIndex(0);
  payload["unit"] = "°C";
  payload["description"] = "Temp. de riel";

  mqttMessage["signalQuality"] = WiFi.RSSI();
  mqttMessage["localIP"] = WiFi.localIP().toString(); 
  mqttMessage["ID"] = SENSOR_NAME;
  mqttMessage["powerLevel"] = voltage;
  mqttMessage["payload"] = payload;
 
  mqttPublish<String>(mainTopic, JSON.stringify(mqttMessage));

  delay(500);
}

/*
String getPublicIP()
{
  String payload;

  HTTPClient http;

  http.begin("http://api.ipify.org");
  int httpCode = http.GET();

  if (httpCode > 0) {
    payload = http.getString();
    Serial.println("Public IP:");
    Serial.println(payload);
  }
  else {
    Serial.print("Connection to http failed\n");
  }

  http.end();

  return payload;
}
*/

