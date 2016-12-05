// standard commands for all PdL devices
#define CMD_GET_DEVICE              0x01
#define CMD_GET_VERSION             0x02

// standard messages for all PdL devices
#define MSG_OK                      0x00
#define MSG_KO                      0xFF

// specific commands - GET
#define CMD_GET_ADDRESS             0x10
#define CMD_GET_BROADCAST_ADDRESS   0x11
#define CMD_GET_UPDATE_FREQUENCY    0x12
#define CMD_GET_SENSOR_READS        0x13

// specific commands - SET
#define CMD_SET_ADDRESS             0x20
#define CMD_SET_BROADCAST_ADDRESS   0x21
#define CMD_SET_UPDATE_FREQUENCY    0x22
#define CMD_SET_SENSOR_READS        0x23

// specific messages
#define MSG_UPDATE_SENSOR_STATUS    0x30
#define MSG_FACTORY_RESET           0x31
