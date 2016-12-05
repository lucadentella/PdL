// Import libraries and definitions
#include <LocoNet.h>
#include <EEPROM.h>
#include "definitions.h"


void setup() {

  // if requested, set the default values in EEPROM
  if(SET_DEFAULTS) {

    EEPROM.write(BASE_ADDRESS_LOCATION, DEFAULT_BASE_ADDRESS);
    EEPROM.write(EXT_ADDRESS_LOCATION, DEFAULT_EXT_ADDRESS);
    EEPROM.write(BROADCAST_ADDRESS_LOCATION, DEFAULT_BROADCAST_ADDRESS);
    EEPROM.write(UPD_FREQUENCY_LOCATION, DEFAULT_UPD_FREQUENCY);
    EEPROM.write(SENSOR_READS_LOCATION, DEFAULT_SENSOR_READS);
  }

  // configure sensor pins and set default sensor state
  for(int i = 0; i < 8; i++) {
    pinMode(sensorPins[i], INPUT_PULLUP);
    lastSensorStates[i] = STATE_UNKNOWN;
  }

  // configure led pin and turn it off
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // confiure button pin
  pinMode(BUTTON_PIN, INPUT);

  // set variable values from EEPROM
  myAddress = EEPROM.read(BASE_ADDRESS_LOCATION) + 256 * EEPROM.read(EXT_ADDRESS_LOCATION);
  broadcastAddress = EEPROM.read(BROADCAST_ADDRESS_LOCATION);
  updateFrequency = EEPROM.read(UPD_FREQUENCY_LOCATION) * 100;
  sensorReads = EEPROM.read(SENSOR_READS_LOCATION);

  // set default working mode 
  workingMode = WRK_NORMAL;
  
  // initialize Loconet library and buffer
  LocoNet.init(LNET_TX_PIN);
  initLnBuf(&LnTxBuffer);

  // initialize serial communication
  Serial.begin(57600);
  Serial.print("PDL OccupancyLNDecoder ");
  Serial.println(VERSION);
  Serial.println();
  Serial.print("My Address: ");
  Serial.println(myAddress);
  Serial.print("Broadcast Address: ");
  Serial.println(broadcastAddress);
  Serial.print("Update Frequency: ");
  Serial.println(updateFrequency);
  Serial.print("Sensor reads: ");
  Serial.println(sensorReads); 
  Serial.println();

  // read and send the initial state of all the sensors
  readSensors();
  sendChangedSensors();
}

void loop() {

  // check if a valid Loconet packet was received
  LnPacket = LocoNet.receive();
  if(LnPacket) LocoNet.processSwitchSensorMessage(LnPacket);

  // check the button
  checkButton();

  // it's time to check the sensors?
  newMillis = millis();
  if((newMillis - lastSensorMillis) > updateFrequency) {

    // read all the sensors and send the changed ones
    readSensors();
    sendChangedSensors();
    lastSensorMillis = newMillis;
  }

  // time to blink the led?
  if(workingMode == WRK_CHANGE_ADDRESS) {
    if((newMillis - lastLedMillis) > CHANGE_ADDRESS_BLINK) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      lastLedMillis = newMillis;
    }
  } else if(workingMode == WRK_CHANGE_BROADCAST) {
    if((newMillis - lastLedMillis) > CHANGE_BROADCAST_BLINK) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      lastLedMillis = newMillis;
    }
  }

  // check if the button was pressed
  checkButton();
}

void checkButton() {

  // is button pressed?
  if(digitalRead(BUTTON_PIN) == LOW) {

    // if we're in normal mode, wait 2 seconds 
    if(workingMode == WRK_NORMAL) {
      
      delay(2000); 
      if(digitalRead(BUTTON_PIN) == HIGH) return;
      digitalWrite(LED_PIN, HIGH);
      while (digitalRead(BUTTON_PIN) == LOW) {}
      workingMode = WRK_CHANGE_ADDRESS;
      Serial.println("Waiting for new address");
    } 
    else {
      
      delay(200);
      if(digitalRead(BUTTON_PIN) == HIGH) return;
      digitalWrite(LED_PIN, LOW);
      while (digitalRead(BUTTON_PIN) == LOW) {}
      workingMode = WRK_NORMAL;      
      Serial.println("Program operation aborted");
    }
  }
}

void readSensors() {

  for(int i = 0; i < 8; i++) {

    // check the actual value of sensor
    int count = 0;
    int sensorState;
    for(int j = 0; j < sensorReads; j++) {
      if(digitalRead(sensorPins[i]) == HIGH) count++;
      delay(5);
    }
    if(count == sensorReads) sensorState = STATE_FREE;
    else sensorState = STATE_BUSY;
    newSensorStates[i] = sensorState;
  }
}

void sendChangedSensors() {

  for(int i = 0; i < 8; i++) {
    if(lastSensorStates[i] != newSensorStates[i]) {
      LocoNet.reportSensor(myAddress + i, newSensorStates[i]);
      lastSensorStates[i] = newSensorStates[i];
      Serial.print("New state for sensor ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(newSensorStates[i]);
    }
  }
}

void sendAllSensors() {

  readSensors();
  for(int i = 0; i < 8; i++)
    LocoNet.reportSensor(myAddress + i, newSensorStates[i]);
}

void notifySwitchRequest( uint16_t Address, uint8_t Output, uint8_t Direction ) {

  if((workingMode == WRK_NORMAL) && (Address == broadcastAddress))
    sendAllSensors();
  
  else if(workingMode == WRK_CHANGE_ADDRESS) {

    myAddress = Address;
    EEPROM.write(BASE_ADDRESS_LOCATION, Address);
    Serial.print("New address: ");
    Serial.println(myAddress);
    workingMode = WRK_CHANGE_BROADCAST;
  }
  else if(workingMode == WRK_CHANGE_BROADCAST) {
    if(Address != myAddress) {
      broadcastAddress = Address;
      EEPROM.write(BROADCAST_ADDRESS_LOCATION, Address);
      Serial.print("New broadcast address: ");
      Serial.println(broadcastAddress);
      workingMode = WRK_NORMAL; 
    }   
  }
}
