// Prototipos

void initializeSerial();
void turnOffSystem();
void initializeNetwork(connectionType currentConnectionType);
void setSleepTime();
bool mqttConnect();
void suscribeMQTT();
void execNormalMode();
void execServerMode();
void mqttCallback(char* topic, byte * payload, unsigned int len);
void initializeMQTT();
void publishTopics();