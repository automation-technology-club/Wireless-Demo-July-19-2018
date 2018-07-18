#include <VirtualWire.h>
#include <EasyTransferVirtualWire.h>

#define BUTTON A0
//Debounce Stuff

int buttonState = LOW;

// the current and previous readings from the input pin
int thisButtonState = LOW;
int lastButtonState = LOW;

// time is measured in milliseconds and will quickly exceed limitations of an integer, so we use a long for these two
unsigned long lastDebounceTime = 0;  // the time the button state last switched
unsigned long debounceDelay = 50;    // the state must remain the same for this many millis to register the button press


//create object
EasyTransferVirtualWire ET; 

struct SEND_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to send
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  //Struct can'e be bigger then 26 bytes for VirtualWire version
  int blinks;
  int pause;
};

//give a name to the group of data
SEND_DATA_STRUCTURE mydata;

void setup(){
  //start the library, pass in the data details
  ET.begin(details(mydata));
  
  // Initialise the IO and ISR
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);	 // Bits per sec
  
  pinMode(13, OUTPUT);
  
  randomSeed(analogRead(0));
  pinMode(BUTTON, INPUT_PULLUP);

}

void loop() {
  
checkButton();

}

void checkButton() {
  //userScheduler.execute(); // it will run mesh scheduler as well
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
        sendMessage();
        Serial.println("Button.");
      }
    }
  }

  // persist for next loop iteration
  lastButtonState = thisButtonState;
  
}

void sendMessage(){
  //this is how you access the variables. [name of the group].[variable name]
  mydata.blinks = 1;
  mydata.pause = 5;
  //send the data
  ET.sendData();
  
  //Just for fun, we will blink it out too
   for(int i = mydata.blinks; i>0; i--){
      digitalWrite(13, HIGH);
      delay(mydata.pause * 100);
      digitalWrite(13, LOW);
      delay(mydata.pause * 100);
    }
  
  //delay(5000);
}

