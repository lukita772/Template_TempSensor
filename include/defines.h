/*
* brief: defines 
*/


#define SENSOR_AREA "Urquiza"
#define SENSOR_NAME "3027"
//#define USE_PANEL // comentar si no usa panel

#define ONE_WIRE_BUS 2 
#define CONNECTION_TIMEOUT 12000L

/*** Memmory MAP ***/
#define EEPROM_ADDR_SLEEPTIME 1
#define EEPROM_ADDR_BOOT MODE EEPROM_ADDR_SLEEPTIME+2

#define ANCII_0 47
#define ANCII_9 58

#define SLEEP_TIME_MAX 180

#define BOOT_MODE_SERVER 2
#define BOOT_MODE_NORMAL 1

#define BOOT_SERVER_TIMEOUT 120000


#ifdef USE_PANEL
  #define BOOT_NORMAL_TIMEOUT 0
#else
  #define BOOT_NORMAL_TIMEOUT 600000 //(ms) 10min 
#endif

#define DATA_LENGTH 2

//#define KEEP_AWAKE 5 //PARA SENSOR. DETECTOR DE LLUVIA


typedef union charByte {
  uint16_t uint16Data;
  byte byteData[DATA_LENGTH];
};

enum connectionType
{
  _default, //wifi->gsm
  _wifi,    //force wifi
  _gprs     //force gsm/gprs
};
