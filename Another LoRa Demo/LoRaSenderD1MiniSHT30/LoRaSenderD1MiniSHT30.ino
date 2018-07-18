/* LeRoy Miller (C) 2018, see license.h for license information */

/*
 *  WeMos D1          RFM9x Module
  GPIO12 (D6) <----> MISO
  GPIO13 (D7) <----> MOSI
  GPIO14 (D5) <----> CLK
  GPIO15 (D8) <----> DIO0/D2 OR DIO1/D3 OR DIO2/D4
  GPIO02 (D4) <----> SEL (Chip Select)
  RST         <----> RST
  * Appears D1, D2, D3, A0, and D0 free
  */
  
#include <SPI.h>
#include <LoRa.h> //https://github.com/sandeepmistry/arduino-LoRa
#include <Wire.h>

#include <WEMOS_SHT3X.h> //https://github.com/wemos/WEMOS_SHT3x_Arduino_Library

#define NODE 1 //Node Number of Sensor
#define SS      D4 
#define RST     22 
#define DI0     D8 
#define BUTTON D3

SHT3X sht30(0x45);

float t, h; //tempature and humidity
int counter;
int buttonState = LOW;

// the current and previous readings from the input pin
int thisButtonState = LOW;
int lastButtonState = LOW;

// time is measured in milliseconds and will quickly exceed limitations of an integer, so we use a long for these two
unsigned long lastDebounceTime = 0;  // the time the button state last switched
unsigned long debounceDelay = 50;    // the state must remain the same for this many millis to register the button press


void setup() {
  Serial.begin(9600);
  pinMode(BUTTON, INPUT_PULLUP);

  while (!Serial);
LoRa.setPins(SS,RST,DI0);
  Serial.println("LoRa Sender");

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
 
}

void loop() {
  getTemp();
 thisButtonState = digitalRead(BUTTON);
  if (thisButtonState != lastButtonState) {
    // reset the timer
    lastDebounceTime = millis();
  }
   if ((millis() - lastDebounceTime) > debounceDelay) {

    // if the button state has changed
    if (thisButtonState != buttonState) {
      buttonState = thisButtonState;

      // only toggle the LED if the buttonState has switched from LOW to HIGH
      if (buttonState == HIGH) {
        
 // getTemp();
  
  sendPacket();
        Serial.println("Button.");
      }
    }
  }

  // persist for next loop iteration
  lastButtonState = thisButtonState;
  
  
  yield();
  //delay(15000);
  //ESP.deepSleep(15*1000000);
}



void getTemp() {

if (sht30.get() == 0) {
  t = sht30.fTemp;
  h = sht30.humidity;
    } else {
  Serial.println("Error!");
  }
}

void sendPacket() {
  Serial.print("Sending packet: ");
  char hex[20];
  //sprintf(hex, "%d %d.%02d %d.%02d",NODE,(int)t,(int)(t*100)%100,(int)h,(int)(h*100)%100);
  sprintf(hex, "%d.%02d %d.%02d",(int)t,(int)(t*100)%100,(int)h,(int)(h*100)%100);
  //sprintf(hex, "%02d %02d",(int)t,(int)h);
  
  // send packet
  LoRa.beginPacket();
  LoRa.print((String)hex);
  LoRa.endPacket();
  //delay(5000);
}
