// Prototipos


//main.cpp
void initializeSerial();
void turnOffSystem();
void initializeNetwork(connectionType currentConnectionType);
void setSleepTime();
void execNormalMode();
void execServerMode();
void mqttCallback(char* topic, byte * payload, unsigned int len);



//mqttApplication.cpp
template<typename T>
void mqttPublish(const char* topic, T data);

bool mqttConnect();
void initializeMQTT();
void publishTopics();
void checkMessages();
void suscribeMQTT();
bool mqttIsConnected();
void mqttSetClient();

#ifdef SENSOR_ISRAINSENSOR
    void st_rainGaugeSensor_counter(int data);
#endif