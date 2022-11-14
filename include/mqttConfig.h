/*
* brief: datos de configuracion necesaria para est. de coneccion prot. mqtt 
*       definir parametros en defines.h
*/

#if defined(SENSOR_NAME) && defined(SENSOR_AREA)
    //MQTT login
    const char* broker = BROKER_IP;
    const uint16_t brokerPort = BROKER_PORT;

    const char MQTT_clientID[] = SENSOR_NAME;
    const char MQTT_user[] = "iot";
    const char MQTT_passworld[] = "iotiot";

    //Topicos
    const char* mainTopic = SENSOR_AREA"/TemperaturaVia/" SENSOR_NAME "/MainData";
    const char* topicSleepTime = SENSOR_AREA"/TemperaturaVia/" SENSOR_NAME "/Sleeptime";
    const char* topicBootMode = SENSOR_AREA"/TemperaturaVia/" SENSOR_NAME "/BootMode";
#else
    #error Debe incluir "defines.h" primero!
#endif
