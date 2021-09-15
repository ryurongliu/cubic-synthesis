#include <dummy.h>


/////*****CODE FOR CONNECTING TO SMARTCUBE ADAPTED FROM PLAYFUL TECHNOLOGY********

/**
 * ESP32-SmartCube
 * Copyright (c) 2019 Playful Technology
 * 
 * ESP32 sketch to connect to Xiaomi "Smart Magic Cube" Rubik's Cube via Bluetooth LE
 * and decode notification messages containing puzzle state.
 * Prints output to serial connection of the last move made, and when the cube is fully
 * solved, triggers a relay output
 */

// INCLUDES
// ESP32 library for Bluetooth LE
#include "BLEDevice.h"

// CONSTANTS
// The MAC address of the Rubik's Cube
// This can be discovered by starting a scan BEFORE touching the cube. Then twist any face
// to wake the cube up and see what new device appears
static BLEAddress *pServerAddress = new BLEAddress("C0:FA:27:35:A0:0E");
// The remote service we wish to connect to
static BLEUUID serviceUUID("0000aadb-0000-1000-8000-00805f9b34fb");
// The characteristic of the remote service we want to track
static BLEUUID charUUID("0000aadc-0000-1000-8000-00805f9b34fb");
// The following constants are used to decrypt the data representing the state of the cube
// see https://github.com/cs0x7f/cstimer/blob/master/src/js/bluetooth.js
const uint8_t decryptionKey[] = {176, 81, 104, 224, 86, 137, 237, 119, 38, 26, 193, 161, 210, 126, 150, 81, 93, 13, 236, 249, 89, 235, 88, 24, 113, 81, 214, 131, 130, 199, 2, 169, 39, 165, 171, 41};
// This pin will have a HIGH pulse sent when the cube is solved
//const byte relayPin = 33;
// This is the data array representing a solved cube
const byte solution[16] = {0x12,0x34,0x56,0x78,0x33,0x33,0x33,0x33,0x12,0x34,0x56,0x78,0x9a,0xbc,0x00,0x00};

const int solved[48] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,32, 33, 34, 35, 36, 37, 38,
                        39, 40, 41, 42, 43, 44, 45, 46, 47, 48};
//1-8 white (UP)
//9-16 red (LEFT)
//17-24 blue (FRONT)
//25-32 orange (RIGHT)
//33-40 green (BACK)
//41-48 yellow (DOWN)

//face-turn generators, in disjoint cycle notation:
const int cycles[6][5][4]= {
  {{17, 19, 24, 22},{18, 21, 23, 20},{6, 25, 43, 16}, {7, 28, 42, 13},{8, 30, 41, 11}}, //F
  {{41, 43, 48, 46}, {42, 45, 47, 44}, {14, 22, 30, 38}, {15, 23, 31, 39}, {16, 24, 32, 40}}, //D
  {{25, 27, 32, 30}, {26, 29, 31, 28}, {3, 38, 43, 19}, {5, 36, 45, 21}, {8, 33, 48, 24}}, //R
  {{1, 3, 8 ,6}, {2, 5, 7, 4}, {9, 33, 25, 17}, {10, 34, 26, 18}, {11, 35, 27, 19}}, //U
  {{9, 11, 16, 14}, {10, 13, 15, 12}, {1, 17, 41, 40}, {4, 20, 44, 37}, {6, 22, 46, 35}}, //L
  {{33, 35, 40, 38}, {34, 37, 39, 36}, {3, 9, 46, 32}, {2, 12, 47, 29}, {1, 14, 48, 27}}, //B
};


// GLOBALS
// Have we found a cube with the right MAC address to connect to?
static boolean deviceFound = false;
// Are we currently connected to the cube?
static boolean connected = false;
// Properties of the device found via scan
static BLEAdvertisedDevice* myDevice;
// BT characteristic of the connected device
static BLERemoteCharacteristic* pRemoteCharacteristic;

int *state;

// HELPER FUNCTIONS
/**
 * Return the ith bit from an integer array
 */
int getBit(uint8_t* val, int i) {
  int n = ((i / 8) | 0);
  int shift = 7 - (i % 8);
  return (val[n] >> shift) & 1;    
}

/**
 * Return the ith nibble (half-byte, i.e. 16 possible values)
 */
uint8_t getNibble(uint8_t val[], int i) {
  if(i % 2 == 1) {
    return val[(i/2)|0] % 16;
  }
  return 0|(val[(i/2)|0] / 16);
}

// CALLBACKS
/**
 * Callbacks for devices found via a Bluetooth scan of advertised devices
 */
class AdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  // The onResult callback is called for every advertised device found by the scan  
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    // Print the MAC address of this device
    Serial.print(" - ");
    Serial.print(advertisedDevice.getAddress().toString().c_str());
    // Does this device match the MAC address we're looking for?
    if(advertisedDevice.getAddress().equals(*pServerAddress)) {
      // Stop scanning for further devices
      advertisedDevice.getScan()->stop();
      // Create a new device based on properties of advertised device
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      // Set flag
      deviceFound = true;
      Serial.println(F(" - Connecting!"));
    }
    else {
      Serial.println(F("... MAC address does not match"));
    }
  }
};

/**
 * Callbacks for device we connect to
 */
class ClientCallbacks : public BLEClientCallbacks {
  // Called when a new connection is established
  void onConnect(BLEClient* pclient) {
    digitalWrite(LED_BUILTIN, HIGH);
    connected = true;
  }
  // Called when a connection is lost
  void onDisconnect(BLEClient* pclient) {
    digitalWrite(LED_BUILTIN, LOW);
    connected = false;
  }
};

/**
 * Called whenever a notication is received that the tracked BLE characterisic has changed
 */
static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  // DECRYPT DATA
  // Early Bluetooth cubes used an unencrypted data format that sent the corner/edge indexes as a 
  // simple raw string, e.g. pData for a solved cube would be 1234567833333333123456789abc000041414141
  // However, newer cubes e.g. Giiker i3s encrypt data with a rotating key, so that same state might be
  // 706f6936b1edd1b5e00264d099a4e8a19d3ea7f1 then d9f67772c3e9a5ea6e84447abb527156f9dca705 etc.
  
  // To find out whether the data is encrypted, we first read the penultimate byte of the characteristic data. 
  // As in the two examples above, if this is 0xA7, we know it's encrypted
  bool isEncrypted = (pData[18] == 0xA7);

  // If it *is* encrypted...
  if(isEncrypted) {
    // Split the last byte into two 4-bit values 
    int offset1 = getNibble(pData, 38);
    int offset2 = getNibble(pData, 39);

    // Retrieve a pair of offset values from the decryption key 
    for (int i=0; i<20; i++) {
      // Apply the offset to each value in the data
      pData[i] += (decryptionKey[offset1 + i] + decryptionKey[offset2 + i]);
    }
  }

  // First 16 bytes represent state of the cube - 8 corners (with 3 orientations), and 12 edges (can be flipped)
  // Byte 17 represents the last twist made - first half-byte is face, and second half-byte is direction of rotation
  //Serial.print("Current State: ");

  //print current state
    for (int i=0; i<34; i++) {
      Serial.print(getNibble(pData, i));
      Serial.print(" ");
    }

    if (memcmp(pData, solution, 16) == 0) { //if solved, print 1 + set solved state
      Serial.print(1);
      Serial.print(" ");
      solveState();
    }
    else //if not solved, print 0 and do the turn 
    {
      Serial.print(0);
      Serial.print(" ");
      int lastMoveFace = getNibble(pData, 32);
      int lastMoveDirection = getNibble(pData, 33);
      turn(lastMoveFace, lastMoveDirection);
    }
    for (int i = 0; i < 48; i++)
    {
      Serial.print(String(state[i]) + " ");
    }
    Serial.println("");
  
}


//TURN THE FACE (represented in state array)
void turn(int turnFace, int turnDirection)
{
  //if turnDirection = 1, clockwise: left to right
  //if turnDirection != 1, anti-clockwise: right to left
  //use cycles[turnFace-1][][]
  if(turnDirection == 1) //clockwise, left to right
  {
    for (int i = 0; i < 5; i++)
    {
      int temp = state[cycles[turnFace-1][i][3]-1]; //last element in cycle
      state[cycles[turnFace-1][i][3]-1] = state[cycles[turnFace-1][i][2]-1]; //copy third element to fourth position 
      state[cycles[turnFace-1][i][2]-1] = state[cycles[turnFace-1][i][1]-1]; //copy second element to third position
      state[cycles[turnFace-1][i][1]-1] = state[cycles[turnFace-1][i][0]-1]; //copy first element to second position
      state[cycles[turnFace-1][i][0]-1] = temp;                              //copy last element to first position
    }
  }
  if(turnDirection != 1) //anti-clockwise, right to left
  {
    for (int i = 0; i < 5; i++)
    {
      int temp = state[cycles[turnFace-1][i][0]-1]; //first element in cycle
      state[cycles[turnFace-1][i][0]-1] = state[cycles[turnFace-1][i][1]-1]; //copy second element to first positon
      state[cycles[turnFace-1][i][1]-1] = state[cycles[turnFace-1][i][2]-1]; //copy third element to second position
      state[cycles[turnFace-1][i][2]-1] = state[cycles[turnFace-1][i][3]-1]; //copy fourth element to third position
      state[cycles[turnFace-1][i][3]-1] = temp;                              //copy first element to fourth position
    } 
  }
}

//output cube state in numbers
void printStateCubelikeNums()
{
  char strBuf1[100];
  sprintf(strBuf1, "            | %2d %2d %2d |", state[0], state[1], state[2]);
  Serial.println(strBuf1);
  sprintf(strBuf1, "            | %2d %2d %2d |", state[3], 0, state[4]);
  Serial.println(strBuf1);
  sprintf(strBuf1, "            | %2d %2d %2d |", state[5], state[6], state[7]);
  Serial.println(strBuf1);
  sprintf(strBuf1, " | %2d %2d %2d | %2d %2d %2d | %2d %2d %2d | %2d %2d %2d |", state[8], state[9], state[10],
          state[16], state[17], state[18], state[24], state[25], state[26], state[32], state[33], state[34]);
  Serial.println(strBuf1);
  sprintf(strBuf1, " | %2d %2d %2d | %2d %2d %2d | %2d %2d %2d | %2d %2d %2d |", state[11], 0, state[12],
          state[19], 0, state[20], state[27], 0, state[28], state[35], 0, state[36]);
  Serial.println(strBuf1);
  sprintf(strBuf1, " | %2d %2d %2d | %2d %2d %2d | %2d %2d %2d | %2d %2d %2d |", state[13], state[14], state[15],
          state[21], state[22], state[23], state[29], state[30], state[31], state[37], state[38], state[39]);
  Serial.println(strBuf1);

  sprintf(strBuf1, "            | %2d %2d %2d |", state[40], state[41], state[42]);
  Serial.println(strBuf1);
  sprintf(strBuf1, "            | %2d %2d %2d |", state[43], 0, state[44]);
  Serial.println(strBuf1);
  sprintf(strBuf1, "            | %2d %2d %2d |", state[45], state[46], state[47]);
  Serial.println(strBuf1);
}

//output cube state in colors
void printStateCubelikeColors()
{
  char strBuf1[100];
  sprintf(strBuf1, "            | %s %s %s |", toColor(state[0]), toColor(state[1]), toColor(state[2]));
  Serial.println(strBuf1);
  sprintf(strBuf1, "            | %s %s %s |", toColor(state[3]), "0", toColor(state[4]));
  Serial.println(strBuf1);
  sprintf(strBuf1, "            | %s %s %s |", toColor(state[5]), toColor(state[6]), toColor(state[7]));
  Serial.println(strBuf1);
  sprintf(strBuf1, " | %s %s %s | %s %s %s | %s %s %s | %s %s %s |", toColor(state[8]), toColor(state[9]), toColor(state[10]),
          toColor(state[16]), toColor(state[17]), toColor(state[18]), toColor(state[24]), toColor(state[25]), toColor(state[26]), 
          toColor(state[32]), toColor(state[33]), toColor(state[34]));
  Serial.println(strBuf1);
  sprintf(strBuf1, " | %s %s %s | %s %s %s | %s %s %s | %s %s %s |", toColor(state[11]), "0", toColor(state[12]),
          toColor(state[19]), "0", toColor(state[20]), toColor(state[27]), "0", toColor(state[28]), toColor(state[35]), "0", toColor(state[36]));
  Serial.println(strBuf1);
  sprintf(strBuf1, " | %s %s %s | %s %s %s | %s %s %s | %s %s %s |", toColor(state[13]), toColor(state[14]), toColor(state[15]),
          toColor(state[21]), toColor(state[22]), toColor(state[23]), toColor(state[29]), toColor(state[30]), toColor(state[31]),
          toColor(state[37]), toColor(state[38]), toColor(state[39]));
  Serial.println(strBuf1);
  sprintf(strBuf1, "            | %s %s %s |", toColor(state[40]), toColor(state[41]), toColor(state[42]));
  Serial.println(strBuf1);
  sprintf(strBuf1, "            | %s %s %s |", toColor(state[43]), "0", toColor(state[44]));
  Serial.println(strBuf1);
  sprintf(strBuf1, "            | %s %s %s |", toColor(state[45]), toColor(state[46]), toColor(state[47]));
  Serial.println(strBuf1);
}

//helper function
String toColor(int facelet)
{
  int face = int((facelet-1)/8);
  
  if (face == 0)
    return "w";
  else if (face == 1)
    return "r";
  else if (face == 2)
    return "b";
  else if (face == 3)
    return "o";
  else if (face == 4)
    return "g";
  else if (face == 5)
    return "y";
  return "n";
}

/*
 * Connect to the BLE server of the correct MAC address
 */
bool connectToServer() {
    Serial.print(F("Creating BLE client... "));
    BLEClient* pClient = BLEDevice::createClient();
    delay(500);
    Serial.println(F("Done."));

    Serial.print(F("Assigning callbacks... "));
    pClient->setClientCallbacks(new ClientCallbacks());
    delay(500);
    Serial.println(F(" - Done."));

    // Connect to the remove BLE Server.
    Serial.print(F("Connecting to "));
    Serial.print(myDevice->getAddress().toString().c_str());
    Serial.print(F("... "));
    pClient->connect(myDevice);
    delay(500);
    Serial.println(" - Done.");
    
    // Obtain a reference to the service we are after in the remote BLE server.
    Serial.print(F("Finding service "));
    Serial.print(serviceUUID.toString().c_str());
    Serial.print(F("... "));
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    delay(500);
    if (pRemoteService == nullptr) {
      Serial.println(F("FAILED."));
      return false;
    }
    Serial.println(" - Done.");
    delay(500);

    // Obtain a reference to the characteristic in the service of the remote BLE server.
    Serial.print(F("Finding characteristic "));
    Serial.print(charUUID.toString().c_str());
    Serial.print(F("... "));
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.println(F("FAILED."));
      return false;
    }
    Serial.println(" - Done.");
    delay(500);
    
    Serial.print(F("Registering for notifications... "));
    if(pRemoteCharacteristic->canNotify()) {
      pRemoteCharacteristic->registerForNotify(notifyCallback);
      Serial.println(" - Done.");
    }
    else {
      Serial.println(F("FAILED."));
      return false;
    }

    Serial.println("READY!");
}

/**
 * Search for any advertised devices
 */
void scanForDevices(){
  Serial.println("Scanning for Bluetooth devices...");
  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 30 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(30);
}

void solveState(){
  for (int i = 0; i < 48; i++)
    state[i] = solved[i];
}

// Initial setup
void setup() {
  // Start the serial connection to be able to track debug data
  Serial.begin(115200);
  
  Serial.print("Initialising BLE...");
  BLEDevice::init("");
  delay(500);
  Serial.println(F("Done."));

  // relayPin will be set HIGH when the cube is solved
 // pinMode(relayPin, OUTPUT);
 // digitalWrite(relayPin, LOW);

  // ledPin will be set HIGH when cube is connected 
  pinMode(LED_BUILTIN, OUTPUT);

  state = (int *) malloc(sizeof(int) *48);
  solveState();
}

// Main program loop function
void loop() {
  // If the cube has been found, connect to it
  if (deviceFound) {
    if(!connected) {
      connectToServer();
    }
  }
  else {
    scanForDevices();
  }
  // Introduce a little delay
  delay(1000);
}
