#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

/////////
//const int buttonPin = 2;    
//boolean running = false;
const int ledPin2 = 6;  
const int ledPin = 8;
const int ledPin3 = 5;  
 int x = 1;
// variables will change:
//int buttonState = 0;         

// This will store the last known state of the button
//int oldButtonState = HIGH;
///////////////////


 
void setup() 
{
  //Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  //Serial.println("Approximate your card to the reader...");
  //Serial.println();
  pinMode(ledPin2, OUTPUT);  
  //pinMode(buttonPin,INPUT_PULLUP);   
  pinMode(ledPin, OUTPUT); 
  pinMode(ledPin3, OUTPUT); 

}
void loop() 
{
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  //Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     //Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  //Serial.println();
  //Serial.print("Message : ");
  content.toUpperCase();
  if ((content.substring(1) == "09 FE 47 B8") || (content.substring(1) == "CD 7D D9 A3")) //change here the UID of the card/cards that you want to give access
  {
    if (x == 0) {
      // Toggle on
      digitalWrite(ledPin, HIGH);
      delay(1000);
      digitalWrite(ledPin, LOW);
      //Serial.println("led 13");
      x = 1;
      }
      else {
      // Toggle off
      digitalWrite(ledPin2, HIGH);
      delay(1000);
      digitalWrite(ledPin2, LOW);
      //Serial.println("led 12");
      x = 0;
    }
  

    // Store the button's state so we can tell if it's changed next time round

   
    //Serial.println("Authorized access");
   
    //digitalWrite(ledPin, HIGH);
    //Serial.println();
    //delay(1000);
  }
  
 
 else   {
    //Serial.println(" Access denied");
    digitalWrite(ledPin3, HIGH);
    delay(1000);
    digitalWrite(ledPin3, LOW);
  }
} 
