#define Door_Security_Version "1.14"
// added option for user and password on mqtt connection - NOT ACTIVE YET.
// Changes ------------
//
// V - 1.14 Changes IN PROGRESS: (12/11/17)
//      - new library:
//        - NetEeprom (working: Mac to eeprom from hardcode)
//        - Eeprom
//      - GOAL: Setup working on webbased eeeprom configuration including:
//        - NetEeprom: Network settings (mac, ip, dns, gateway)
//        - MQTT Settings (server, doors array, mqtt_clientID)
//        - Debug value
// V - 1.13 Changes: (4/26/17)
//      - added publish mqtt_clientID in SETUP
//      - added publish MQTT IP address in SETUP
// V - 1.12 Changes: (4/14/17)
//      - added var mqtt_clientID - to give each door unit a unique ID
//      - changed reconnect MQTT client to function that runs at 5sec intervals without breaking loop.
//        - prevents the continual battering of the mqtt protocol on the server, give time to connect.
//  Updated Libraries :
//    SPI = 1.0.6
//    Ethernet = 1.1.2
//    PubSubClient = 2.6.0
//
//      Created board for 4 doors.
// V - 1.11 Changes: (May 10)
//    changed mqtt_doors to array
//      - make a array to track if door is manually opened and restart the interval.
//      - change time function to work with door arry
//      - make door array include pin numbers for door laches.
//    Changed Mqtt info.
//      - Subscribed to the "security/door/+/sub" - this will see all door info but will filter it based on the mqtt array
//      - checks if mqtt door request is the door in the mqtt_doors array
//
// V - 1.10 Changes: (April 16)
//    Added multiple door possibilities for single arduino
//    - Started with 2 doors.
//    - testing with door's 1,2
//
//
// 1.09 changes: (April 4, 2016)
//      Changed from Relay to N-channel Mosfet
//      http://bildr.org/2012/03/rfp30n06le-arduino/
//      USES no optoisolator
//      Result: success No more ethernetshield resetting.
//      Additional mod's/ changes:
//        Changed reset from pin reset to software : void(* resetFunc) (void) = 0;  // declare reset fuction at address 0
//        Removed Temp option
//
// 1.08 changes: (Dec 18, 2015)
// Changed Pins: (To not interfere with SD pin 4 if wanting later.)
//       relay pin to pin 5
//       Temp Pin to pin 3
// Added Wire from pin 9 to RESET to reset if not connecting.
// Updated Libraries :
//    SPI = 1.0.6
//    Ethernet = 1.1.1
//    PubSubClient = 2.4.0
//    DallasTemerature = 3.7.2 BETA
//    OneWire = 2.3.1
// Ethernet IP = DHCP
//
// 1.07 changes:
// Slim down code to:
//    - Publish to MQTT in setup and then only respond to a MQTT subscription in function callback NOT in function loop.
//
// 1.06 changes:
// adjusted open time so it is time based rather than Delay based.
// Adjusted reset time to 2.15 minutes instead of 5mins.

//Issues
//  When starting it does a manual open from Mqtt

//Resolved
//  Issues I have a close signal from the server and from the arduino tryin to run on manual open. (turned Off openhab close mqtt after manual open

// 1.05 changes:
// mqtt format changed to : "security/door/[door number]/[pub | sub | rfid | in_temp | version]"
// gave language variables for mqtt communication.


//                    **** EDIT BELOW ****
// ***** ________________________________________________ *****


//#define relay 5 // relay =  define analoge or digital pin that is controlling the relay
//int relays[] = {5, 6}; //mqtt_doors[0] == 2, mqtt_doors[1] == 4
//int mqtt_doors[] = {4,5}; //mqtt_doors[0] == 2, mqtt_doors[1] == 4


// 1 doors *****************  //
const int mqtt_doors_rows = 1;
const int mqtt_doors_cols = 4;
long mqtt_doors[mqtt_doors_rows][mqtt_doors_cols] = {  // 4 columns = {door number, pin number, isopen (1=yes), opening timestamp in miliseconds}  Zero out when resetting door.
  {21, 5, 0, 0}
};

// 4 doors ***************** //
//const int mqtt_doors_rows = 4;
//const int mqtt_doors_cols = 4;
//long mqtt_doors[mqtt_doors_rows][mqtt_doors_cols] = {  // 4 columns = {door number, pin number, isopen (1=yes), opening timestamp in miliseconds}  Zero out when resetting door.
//  {1, 5, 0, 0},
//  {2, 6, 0, 0},
//  {4, 7, 0, 0},
//  {5, 8, 0, 0}
//};


#define Debug 1 // 1 for Serial Print; 0 for no serial.

// ***** ----------   OptoIsolator on Relay ?? ----------
//  uncomment below if no optoisolator  // LOW = locked = OFF
#define Relay_ON HIGH
#define Relay_OFF LOW

// Uncomment below if using optoisolation  // LOW = unlocked = ON
//#define Relay_ON LOW
//#define Relay_OFF HIGH



// ***** ----- MQTT SETTING ------
//#define mqtt_door "2"
//#define mqtt_door "4"
int door = 0;
const char* mqtt_all_doors = "+";

//const char* mqtt_clientID = "FLC";
const char* mqtt_clientID = "FLC_test";
//const char* mqtt_clientID = "MainBuildingUnit-1";

// ***** ----- NETWORK SETTINGS -----
//        Update these with values suitable for your network.
byte mac[6]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xFE };  // mac     = Give Unique MAC address. (change the end of the mac by adding 1 letter.)


//  "0xEC" = Door 1,2,4,5 unit "Main Building - 1"
//  "0xEB" = Door 1,2,4,5 unit "Main Building"
//  "0xEA" = door 3  Acad Building
//  "0xFE" = FLC Building
//
//  "0xEF" = Unit #4 testing

//const char* mqtt_user = "";
//const char* mqtt_pass = "";


//byte server[] = { 192, 169, 0, 99 };                        // server  = List Server Device IP
byte server[] = { 192, 169, 0, 50 };                        // server  = List Server Device IP 
//char port = 1883;
//byte ip[]     = { 192, 169, 0, 53 };                      // ip      = Give Unique IP to device
//
//IPAddress server(192, 169, 0, 50);
//IPAddress ip(192, 169, 0, 51);

// ***** ----- Language Settings -------
const char* lang_locked = "locked";
const char* lang_unlocked = "unlocked";
const char* lang_man_unlocked = "Man. Unlocked";
const char* lang_reset = "reset";


//                  DO NOT EDIT BELOW LINE
// ***** _________________________________________________ *****

//includes
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <NetEEPROM.h>
#include <EEPROM.h>

// #include <avr/wdt.h>   // Include the watch-dog library functions

//#define mqtt_base "security/door/"
const char* mqtt_base = "security/door/";
const char* mqtt_pub = "/pub";
const char* mqtt_sub = "/sub";
const char* mqtt_pub_rfid = "/rfid";
const char* mqtt_pub_temp = "/in_temp";
const char* mqtt_pub_version = "/version";
const char* mqtt_pub_clientID = "/clientID";
const char* mqtt_pub_IP = "/ip";

char* mqtt_text;

#define def_auto_open "Open"
#define def_manual_open "man"

//String stringOne, stringTwo, stringThre;
String message_str, Software_version;

char message_buff[100];
long previousMillis = 0;       // will store last time LED was updated
long previousMqtt = 0;
long previousOpen = 0;  //into matrix
long intervalOpen = 5; // in Seconds
long intervalMqtt = 90000; //(135000 = 2:15 min, 90000 = 1.5 Mins)  miliSeconds of disonnection from openhab before restart and auto lock door.
long interval = 30000;    // (30 Sec) interval at which to send door status to Openhab (milliseconds)
int isDisconnected = 0;
int isOpen = 0;
int reconnect = 0;
long lastReconnectAttempt = 0;

//int sentversion = 0;

//////////////////------------------------ getValue SPlit String ------------------------///////////////////////////
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


int time_func(String type, int ReSeT,  long previous, long interval, int door ) { // time_func( STRING type = "millis" or "mqtt", INT ReSeT [1 = reset millis or mqtt to current - else - normal time check] ,LONG previous[millis or mqtt], LONG interval(millis or mqtt))
  boolean result;
  unsigned long current = millis();
  if (ReSeT == 1) {
    if (type == "mqtt") {
      previousMqtt = current;
    } else if (type == "millis") {
      previousMillis = current;
    } else if (type == "open") {
      mqtt_doors[door][3] = current;
    }
  } else {
    if (current - previous > interval) {
      if (Debug == 1) {
        Serial.println("F:time - cur " + type + ": " + current);
        Serial.println("F:time - prev" + type + ": "  + previous);
        Serial.println("F:time - int" + type + ": " + interval);
      }
      if (type == "mqtt") {
        previousMqtt = current;
      } else if (type == "millis") {
        previousMillis = current;
      } else if (type == "open") {
        mqtt_doors[door][3] = current;
      }
      result = 1;
    } else {
      result = 0;
    }
    return result;
  }
}


EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

//////////////////------------------------ RESET FUNCTION ------------------------///////////////////////////
void(* resetFunc) (void) = 0; //declare reset function @ address 0

//////////////////------------------------ getcharArray Setup mqtt string ------------------------///////////////////////////
char* getcharArray(String stOne, String stTwo, String stThree = " " ) {
  memset( &message_buff, 0, 100 );
  String pubString = "";

  if (stThree == " ") {
    pubString = stOne + stTwo;
  } else {
    pubString = stOne + stTwo + stThree;
  }
  pubString.toCharArray(message_buff, pubString.length() + 1);
  if (Debug == 1) {
    //    Serial.println( "getcharArray: " + String(message_buff));
  }
  return message_buff;
}


boolean reconnectmqtt() {
  if (client.connect(mqtt_clientID)) {
    //    client.connect( mqtt_base, mqtt_user, mqtt_pass);
    if (Debug == 1) {
      Serial.println("F:Reconn..." + int(reconnect));
    }
    for (int mqtt_init = 0; mqtt_init < mqtt_doors_rows; mqtt_init++) {
      mqtt_text = getcharArray(mqtt_base, String(mqtt_doors[mqtt_init][0]), " ");
      client.publish( ( char*) mqtt_text, (char*) "alive!");
      if (Debug == 1) {
        //                Serial.print("L: connected to: " + String(mqtt_init));
        //                Serial.println(" - whole text : " + String(mqtt_text) );
        Serial.println("F:Recon-cnct to: " + String(mqtt_text));
      }
      mqtt_text = getcharArray(mqtt_base, mqtt_all_doors, mqtt_sub);
      if (Debug == 1) {
        Serial.println("F:Recon-sub to " + String(mqtt_text));
      }
      client.subscribe(mqtt_text);
    }
    //    if (client.connected()) {
    //      reconnect = 0;
    //      if (Debug == 1) {
    //        Serial.println("LF: Conected " + String(mqtt_text));
    //      }
    //    } else {
    //      ++reconnect;
    //    }
    // Once connected, publish an announcement...
    //    /client.publish("security/door/test","hello world");
    // ... and resubscribe
    //    cli/ent.subscribe("security/door/1");
  }
  return client.connected();
}


//////////////////------------------------ OpenDoor ------------------------///////////////////////////
void OpenDoor(int door) {
  mqtt_text = getcharArray(mqtt_base, String(mqtt_doors[door][0]), " ");
  if (Debug == 1) {
    //    Serial.print("Function OpenDoor: Opening Door " +  String(mqtt_text));
  }
  client.publish((char*)mqtt_text, lang_unlocked);
  if (Debug == 1) {
    //    Serial.print ( "Function Callback: Send Mqtt unlocked  = ");
  }
  digitalWrite(mqtt_doors[door][1], Relay_ON); // Unlock door!
}


//////////////////------------------------ CloseDoor ------------------------///////////////////////////
void CloseDoor(int door) {
  mqtt_text = getcharArray(mqtt_base, String(mqtt_doors[door][0]), " ");
  if (Debug == 1) {
    Serial.print("F:CloseDoor - relay" + String(mqtt_text));
  }
  client.publish( (char*) mqtt_text, lang_locked);
  mqtt_doors[door][2] = 0; // set door isopen
  mqtt_doors[door][3] = 0; // time zero'd
  digitalWrite(mqtt_doors[door][1], Relay_OFF); // Unlock door!
}


//////////////////------------------------ OpenDoor MANUAL ------------------------///////////////////////////
void OpenDoorMan(int door) {
  mqtt_text = getcharArray(mqtt_base, String(mqtt_doors[door][0]));
  if (Debug == 1) {
    Serial.println("F:OpenDoorMan - " + String(mqtt_text));
  }

  digitalWrite(mqtt_doors[door][1], Relay_ON); // Unlock door!
  //  for(int openi = 0; openi < 3; openi++){
  mqtt_doors[door][2] = 1; // set door isopen
  mqtt_doors[door][3] = millis(); // Set opentime at current Millseconds

}


//////////////////------------------------ Callback ------------------------///////////////////////////
void callback(char* topic, byte* payload, unsigned int length) {
  int i = 0;
  memset( &message_buff, 0, 100 );
  String Topic = String(topic);
  if (Debug == 1) {
    Serial.print("F:Call-Recvd at: [" + String(topic) + "]  | ");
    //    Serial.println("Function Callback: Length: " + String(length, DEC));

  }
  //  int mqtt_doors_arrSize = arr_len( mqtt_doors );
  int ii;
  for (ii = 0; ii < mqtt_doors_rows; ii++) {
    if (Topic.startsWith(String(mqtt_base) + int(mqtt_doors[ii][0]) + String(mqtt_sub))) {  //security/door/4/sub
      if (Debug == 1) {
        Serial.print("topic Matches | ");
      }
      for (i = 0; i < length; i++) {
        message_buff[i] = payload[i];
      }
      String msgString = String(message_buff);
      if (Debug == 1) {
        Serial.println("FC: Payload: " + msgString);
      }
      String word1 = getValue(msgString, ' ', 0);

      if (word1.equals(def_manual_open)) {
        String setTime = getValue(msgString, ' ', 1);
        char tarray[3];
        setTime.toCharArray(tarray, sizeof(tarray));
        long dhrs = atol(tarray);
        //int dhrs = atoi(tarray);
        if (!dhrs) {
          dhrs = intervalOpen;
        }
        intervalOpen = (dhrs * 1000);
        time_func( "open", 1 , previousOpen, intervalOpen, ii );
        OpenDoorMan(ii);
      } else {
        if (word1.equals(def_auto_open)) {
          OpenDoor(ii);
        } else {
          CloseDoor(ii);

        }
      }

    } else {

      //      Serial.print("NOT THE right Door... ");
      //      Serial.println( String(mqtt_base) + int(mqtt_doors[ii]) + String(mqtt_sub));
    }
  }
  if (Debug == 1) {
    Serial.println("No Match");
  }
  time_func("mqtt", 1, 0 , 0, -1 );
}

void printMac(byte mac[]) {
  for (int i = 0; i < 6; i++) {
    if (i > 0) {
      Serial.print(":");
    }
    if (mac[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
  }
}
//////////////////------------------------ SETUP ------------------------///////////////////////////
void setup() {

  if (Debug == 1) {
    Serial.begin(115200);
    Serial.println("***** Function setup *****");
    Serial.print("MQTT_Client ID: ");
    Serial.println( mqtt_clientID);
  }
  int NotCONN = 0;

  if (NetEeprom.checkMagic()) {
    NetEeprom.readMac(mac);
    if (Debug == 1) {
      Serial.println("Reading mac address.");
    }
  } else {
    NetEeprom.writeDhcpConfig(mac);
    if (Debug == 1) {
      Serial.println("Writing mac address (not set).");
    }
  }
  if (Debug == 1) {
    Serial.print("MAC: "); printMac(mac); Serial.println();
  }
    NetEeprom.begin();
  if (Debug == 1) {
    Serial.println("NetEeprom begin...");
  }

  if (Debug == 1) {
    Serial.print("Ethernet Connected: ");
    //    Serial.println(Ethernet.localIP());
  }

  for (int thisPin = 0; thisPin < mqtt_doors_rows; thisPin++) {
    pinMode(mqtt_doors[thisPin][1], OUTPUT);
    digitalWrite(mqtt_doors[thisPin][1], Relay_OFF); // Make sure door is locked
  }
  delay(1000);

  //  digitalWrite(resetPin, HIGH);
  //  pinMode(resetPin, OUTPUT);

  //  delay(500);/



  NotCONN = 0;
  //  while (!client.connect( mqtt_base, mqtt_user, mqtt_pass)) {
  while (!client.connect( mqtt_clientID )) {
    if (NotCONN == 5) {   //time is up reset door
      if (Debug == 1) {
//        Serial.println( "Reset" );
      }
//      client.disconnect();
//      resetFunc();
//      //      digitalWrite(resetPin, LOW);
    }
      ++NotCONN;
      if (Debug == 1) {
        Serial.print( NotCONN );
        Serial.println("nd attempt to conect to mqtt");
//        Serial.println("MQTT state: " + client.state());
  
      }
    
  }
  mqtt_text = getcharArray(mqtt_base, mqtt_all_doors, mqtt_sub);
  if (Debug == 1) {
    Serial.println("connected to: " + String(mqtt_text));
  }
  client.subscribe(mqtt_text);
  if (Debug == 1) {
    Serial.print("Subscribed to:");
    Serial.println(mqtt_text);
    Serial.print("Version:");
    Serial.println(Door_Security_Version);
  }
  for (int mqtt_init = 0; mqtt_init < mqtt_doors_rows; mqtt_init++) {
    mqtt_text = getcharArray(mqtt_base, String(mqtt_doors[mqtt_init][0]), mqtt_pub_version);
    client.publish( ( char*) mqtt_text, (char*) Door_Security_Version);

    mqtt_text = getcharArray(mqtt_base, String(mqtt_doors[mqtt_init][0]), mqtt_pub_clientID);
    client.publish( ( char*) mqtt_text, (char*) mqtt_clientID);

    mqtt_text = getcharArray(mqtt_base, String(mqtt_doors[mqtt_init][0]), mqtt_pub_IP); mqtt_text = getcharArray(mqtt_base, String(mqtt_doors[mqtt_init][0]), mqtt_pub_version);
    client.publish( ( char*) mqtt_text, (char*) Door_Security_Version);
    // String IPaddy =  Ethernet.localIP().toString();
    String ip = String (Ethernet.localIP()[0]);
    ip = ip + ".";
    ip = ip + String (Ethernet.localIP()[1]);
    ip = ip + ".";
    ip = ip + String (Ethernet.localIP()[2]);
    ip = ip + ".";
    ip = ip + String (Ethernet.localIP()[3]);
    //const char* display_IP = String(IPaddy[0]) + "." + String(IPaddy[1]) + "." + String(IPaddy[2]) + "." + String(IPaddy[3]);
    client.publish( ( char*) mqtt_text, ip.c_str());

    if (Debug == 1) {
      Serial.print("connected to:");
      Serial.println((char*) mqtt_text);

          Serial.print("IPaddy:");
          Serial.println(ip);
    }
  }
  lastReconnectAttempt = 0;


}


void DisplayDoorarray() {
  for (int i = 0; i < mqtt_doors_rows; i++) {                                  //first loop
    if (Debug == 1) {
      Serial.println("ARRAY " + String(i));
    }
    for (int j = 0; j < mqtt_doors_cols; j++) {                         //second loop
      if (Debug == 1) {
        Serial.print("Current Vals - mqtt_doors[");
        Serial.print(String(i));
        Serial.print("][");
        Serial.print(String(j));
        Serial.print("] = ");
        Serial.println(mqtt_doors[i][j]);          //set current pin as OUTPUT
      }
    }
  }
}


//////////////////------------------------ LOOP ------------------------///////////////////////////
void loop() {
  if (!client.connected()) {
    // clientID, username, MD5 encoded password
    if (Debug == 1) {
      Serial.println("F:Loop - MQtt state:" + client.state());
    }

    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnectmqtt()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    WebConfig();
    for (int isopenloop = 0; isopenloop < mqtt_doors_rows; isopenloop++) {
      if (mqtt_doors[isopenloop][2] == 1) { //have isopen check with door array
        time_func("mqtt", 1, 0 , 0, -1 );
        int past_intervalOpen = time_func( "open", 0 , mqtt_doors[isopenloop][3], intervalOpen, isopenloop );
        if (past_intervalOpen == 1) { //time is up relock door
          mqtt_doors[isopenloop][2] = 0;  // clears isopen
          mqtt_doors[isopenloop][3] = 0;  // clears timestamp for open
          CloseDoor(isopenloop);
          if (Debug == 1) {
            Serial.println("F:Loop - close:" + mqtt_doors[isopenloop][0]);
          }
        }
      }
    }

    doorLoop();

  }

  if (isDisconnected == 1) {
    if (Debug == 1) {
      Serial.println(lang_reset);
    }
    //      void disconnect ();
    //    digitalWrite(resetPin, LOW);
    resetFunc();  //call reset
  }

  int past_intervalMqtt = time_func("mqtt", 0 , previousMqtt, intervalMqtt, -1 );
  if (past_intervalMqtt == 1) {
    for (int doornum = 0; doornum < mqtt_doors_rows; doornum++) {
      CloseDoor(mqtt_doors[doornum][0]);
      mqtt_text = getcharArray(mqtt_base, String(mqtt_doors[doornum][0]), " " );
      client.publish((char*) mqtt_text, lang_reset);
      if (Debug == 1) {
        Serial.println("F:Loop - past mqtt pub: " + String(mqtt_text));
      }
    }

    client.disconnect();
    if (Debug == 1) {
      DisplayDoorarray();

      isDisconnected = 1;
    }
  }

}


//////////////////------------------------ DoorLoop ------------------------///////////////////////////
void doorLoop() {
  client.loop();
}




