#define DEVICE_ID       0x01
#define DEVICE_VERSION  0x02

#define LNET_TX_PIN   7
#define LNET_RX_PIN   8
#define BUTTON_PIN    9
#define LED_PIN       13

#define STATE_FREE    0
#define STATE_BUSY    1
#define STATE_UNKNOWN 2

#define BASE_ADDRESS_LOCATION      0
#define EXT_ADDRESS_LOCATION       1
#define BROADCAST_ADDRESS_LOCATION 2
#define UPD_FREQUENCY_LOCATION     3
#define SENSOR_READS_LOCATION      4
 
#define DEFAULT_BASE_ADDRESS       1
#define DEFAULT_EXT_ADDRESS        0
#define DEFAULT_BROADCAST_ADDRESS  255
#define DEFAULT_UPD_FREQUENCY      10
#define DEFAULT_SENSOR_READS       5

#define WRK_NORMAL           0
#define WRK_CHANGE_ADDRESS   1
#define WRK_CHANGE_BROADCAST 2

#define CHANGE_ADDRESS_BLINK    1000
#define CHANGE_BROADCAST_BLINK  200


int sensorPins[8] = {2,3,4,5,A0,A1,A2,A3};
int lastSensorStates[8];
int newSensorStates[8];
int sensorReads;

int myAddress;
int broadcastAddress;
int updateFrequency;
unsigned long lastSensorMillis;
unsigned long lastLedMillis;
unsigned long newMillis;

int workingMode;

static LnBuf LnTxBuffer;
static lnMsg *LnPacket;

