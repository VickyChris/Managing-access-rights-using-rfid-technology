//include librairies
#include <SPI.h>
#include <MFRC522.h>
#include <ThingSpeak.h>
#include <ESP8266WiFi.h>

// Define PINS
#define SS_PIN D8
#define RST_PIN D0
#define LED_PIN D1
#define MQ135_PIN D2

// Varibles
int myChannelNumber = 2 ; //My Channel Number
const char * myWriteAPIKey = "X13239BHCS0K6203"; //My Write API Key
const char* ssid = "Pixel3a"; //My Network SSID
const char* password = "1234567890"; //My Network Password

WiFiClient espClient;//creation of espclient object
void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);//Set the ESP32 as a Wi-Fi station
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
   randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());//printing on serial monitor the connected ip address
}

byte readCard[4];
String MasterTag = "57AEF73F";
String tagID = "";
int sensorValue = 0;
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// Create instances
MFRC522 mfrc522(SS_PIN, RST_PIN);
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

/**********************************************************************************************
 * setup() function
**********************************************************************************************/
void setup() 
{
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Serial.begin(115200);
  SPI.begin();
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  mfrc522.PCD_Init();
  delay(4);
  pinMode(LED_PIN, OUTPUT);
  //pinMode(3, OUTPUT);

  //Show details of PCD - MFRC522 Card Reader
  mfrc522.PCD_DumpVersionToSerial();
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Serial.println("--------------------------");
  Serial.println(" Access Control ");
  Serial.println("Scan Your Card>>");
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// Connect to WIFI
  setup_wifi();
  ThingSpeak.begin(espClient); // initializing the thingspeak with the espclient
}

/**********************************************************************************************
 * loop() function
**********************************************************************************************/
void loop() 
{
  //----------------------------------------------------------------------
  //Wait until new tag is available
  while (getID()) {
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    // Cheking of Master Tag
    if (tagID == MasterTag){
      Serial.println(" Access Granted!");
      Serial.println("--------------------------");
      digitalWrite(LED_PIN, HIGH);
      delay(1000);
      digitalWrite(LED_PIN, LOW);
      delay(1000);
       //You can write any code here like, opening doors, 
       //switching ON a relay, lighting up an LED etc...
    }
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    else{
      Serial.println(" Access Denied!");
      Serial.println("--------------------------");
    }
    
    // Reading of MQ-135 Value
    int sensorValue = analogRead(D2);
    Serial.println("The amount of CO2 in PPM");
    Serial.println(sensorValue);
    delay(2000);
    Serial.println("--------------------------");

    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    delay(2000);
    Serial.println(" Access Control ");
    Serial.println("Scan Your Card>>");
   
  }
  //----------------------------------------------------------------------

//This function returns the code 200 if it has succeeded in publishing the readings.
int x = ThingSpeak.writeField(myChannelNumber, 1, tagID, myWriteAPIKey);
int y = ThingSpeak.writeField(myChannelNumber, 2, sensorValue, myWriteAPIKey);    
  if(x == 200){
    Serial.println(F("Channel update successful."));
    }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));}
}
/**********************************************************************************************
 * getID() function
 * Read new tag if available
**********************************************************************************************/
boolean getID() 
{
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  // Getting ready for Reading PICCs
  //If a new PICC placed to RFID reader continue
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return false;
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //Since a PICC placed get Serial and continue
  if ( ! mfrc522.PICC_ReadCardSerial()) {
  return false;
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  tagID = "";
  // The MIFARE PICCs that we use have 4 byte UID
  for ( uint8_t i = 0; i < 4; i++) {
  //readCard[i] = mfrc522.uid.uidByte[i];
  // Adds the 4 bytes in a single String variable
  tagID.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  tagID.toUpperCase();
  mfrc522.PICC_HaltA(); // Stop reading
  return true;
}
