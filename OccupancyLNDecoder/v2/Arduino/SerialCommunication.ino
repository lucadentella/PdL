// Manage incoming serial packets
void onSerialPacket(const uint8_t* buffer, size_t size)
{

  // switch on command byte
  switch(buffer[0]) {

    case CMD_GET_DEVICE: sendByte(DEVICE_ID); break;
    case CMD_GET_VERSION: sendByte(DEVICE_VERSION); break;
    case CMD_GET_ADDRESS: sendTwoBytes(EEPROM.read(EXT_ADDRESS_LOCATION), EEPROM.read(BASE_ADDRESS_LOCATION)); break;
    case CMD_GET_BROADCAST_ADDRESS: sendByte(EEPROM.read(BROADCAST_ADDRESS_LOCATION)); break;
    case CMD_GET_UPDATE_FREQUENCY: sendByte(EEPROM.read(UPD_FREQUENCY_LOCATION)); break;
    case CMD_GET_SENSOR_READS: sendByte(EEPROM.read(SENSOR_READS_LOCATION)); break;

    case CMD_SET_ADDRESS: setAddress(buffer[1], buffer[2]); break;
    case CMD_SET_BROADCAST_ADDRESS: setBroadcastAddress(buffer[1]); break;
    case CMD_SET_UPDATE_FREQUENCY: setUpdateFrequency(buffer[1]); break;
    case CMD_SET_SENSOR_READS: setSensorReads(buffer[1]); break;
  }
}


// command actions

void sendByte(uint8_t data) {

  uint8_t packet[1];
  packet[0] = data;
  serial.send(packet, 1);
}

void sendTwoBytes(uint8_t data1, uint8_t data2) {

  uint8_t packet[2];
  packet[0] = data1;
  packet[1] = data2;
  serial.send(packet, 2);
}

void setAddress(uint8_t newExtAddress, uint8_t newBaseAddress) {
  
  // store the new address in EEPROM
  EEPROM.write(BASE_ADDRESS_LOCATION, newBaseAddress);
  EEPROM.write(EXT_ADDRESS_LOCATION, newExtAddress);      

  // update the running variable
  myAddress = newBaseAddress + 256 * newExtAddress;

  // return OK
  sendByte(MSG_OK);
}

void setBroadcastAddress(uint8_t newBroadcastAddress) {
  
  // store the new broadcast address in EEPROM
  EEPROM.write(BROADCAST_ADDRESS_LOCATION, newBroadcastAddress);
  
  // update the running variable
  broadcastAddress = newBroadcastAddress;

  // return OK
  sendByte(MSG_OK);  
}

void setUpdateFrequency(uint8_t newUpdateFrequency) {
  
  // store the new broadcast address in EEPROM
  EEPROM.write(UPD_FREQUENCY_LOCATION, newUpdateFrequency);
  
  // update the running variable
  updateFrequency = newUpdateFrequency;

  // return OK
  sendByte(MSG_OK);  
}

void setSensorReads(uint8_t newSensorReads) {
  
  // store the new broadcast address in EEPROM
  EEPROM.write(SENSOR_READS_LOCATION, newSensorReads);
  
  // update the running variable
  sensorReads = newSensorReads;

  // return OK
  sendByte(MSG_OK);  
}

void notifySensorChanged(uint8_t sensorId, uint8_t sensorState) {
  
  uint8_t packet[3];
  packet[0] = MSG_UPDATE_SENSOR_STATUS;
  packet[1] = sensorId;
  packet[2] = sensorState;
  serial.send(packet, 3);
}

