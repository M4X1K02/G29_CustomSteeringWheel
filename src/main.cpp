/*
-----------------------DESCRIPTION----------------------
This sketch is for the MCU that sits in the steering wheel.
It controls the output of the PS4 Circle button and the 6 digit display.

PS4 circle button: GPIO 5 (D1)
Status LED:        GPIO 4 (D2)
Display Clock:     GPIO 12(D6)
Display Data:      GPIO 14(D5)

The signal for the circle button comes from another MCU, which has a wireless P2P ESP Now connection.
Once the remote button is pressed, the MCU receives a packet containing the value "1".
When it is released, another packet with the value "0" is received.
During that time, the circle pin has to be pulled HIGH.
After "0" is received, the circle pin has to be pulled LOW.
A status LED indicates the reception of the transmitted packet and the status of the circle button (HIGH/LOW).

If the time between a "1" and a "0" is greater than 5 seconds, a timer is started which is displayed of the 6 digit display.
To stop the timer and the display, the remote button has to be presseed 3 times in fast succession, so the signal looks like this:
  ______10_10_10_______ (underscore means no packet received)
---------------------END OF DESCRIPTION-----------------


-----------------------BUGS-----------------------------
- gametimer starting faster than it should
- stop because of packetcount
- never runs main even above 125 packetcount
--------------------------------------------------------


-------------------LIBRARIES----------------------------
6 digit tm1637 display
source: https://github.com/TinyTronics/TM1637_6D
--------------------------------------------------------

*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <TM1637_6D.h>

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    // char a[32];
    // int b;
    // float c;
    // String d;
    // bool e;
    int8_t f;
} struct_message;
// Create a struct_message called myData
struct_message myData;

// gpios
const int8_t CLK_Display = 12; 
const int8_t DIO_Display = 14; 
const int8_t Status_LED = 4; 
const int8_t PS4_Button = 5; 

// variables
int8_t state = 0; // rising of falling edge of remote button
uint32_t t0 = 0; // rising edge time for 5 second feature
uint32_t t1 = 0; // falling edge time
uint32_t t2 = 0; // storage for t1 for timer feature
uint32_t timediff = 0; // timer value
bool timerstart = false;
int8_t triple_counter = 0; // counter for reset
bool received = false; // flag for received packet

// declare 6 digit tm1637 display 
TM1637_6D tm1637_6D(CLK_Display,DIO_Display);

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  received = true;
  // start timer on press of remote button
  myData.f==1 ? (t0 = millis()) : (t1 = millis());
  Serial.println("Data received");
}

// checks if the remote button has been pressed for longer than 5 seconds
void fiveSecondsPressed() {
  // check only on release of remote button (state=0)
  if (state!=0) {return;}
  if (((millis() - t0) > 5000)) {
    Serial.printf("Held button for %d seconds, starting timer\n", millis() - t0);
    timerstart = true;
    t2 = t1;
  }
}

// detect a press of the remote button in fast succession 3 times
void triplePressReset() {
  if ((t1 - t0 < 333) || (t1 - t0 > -333)) {
    triple_counter++;

    // reset counter and timer.
    if (triple_counter >= 5) {
      timediff = 0;
      triple_counter = 0;
      timerstart = false;
    }
  }else{
    triple_counter = 0;
  }
}

// display timer onto the 6 digit display
void showDisplay(int32_t time) {
  // numbers and decimal points are shown in reverse on the display
  int8_t DispNum [6] = {};
  int8_t DispPnt [6] = {};

  // if the time is 0, just display "------" instead of nothing
  if (time == 0) {
    tm1637_6D.displayError();
  }else{
    int sec = time/1000;
    DispNum[0] = time%1000/100; // milliseconds (in hundrets)
    DispNum[1] = sec%10; //seconds lower digit
    DispNum[2] = sec%60/10; //seconds higher digit
    DispNum[3] = sec/60%10; // minutes lower digit
    DispNum[4] = sec/600; // minutes higher digit
    DispNum[5] = sec/3600; // hours lower digit

    //display as long as sec < 600(10min):  min[0]: [] : sec[1] : sec[0] : [] : milli[0]
    if (sec<600) {
      DispNum[5] = DispNum[3];
      DispNum[4] = 10; //empty
      DispNum[3] = DispNum[2];
      DispNum[2] = DispNum[1];
      DispNum[1] = 10;
      // all decimal points turned of for sec < 600
      for (int i=0; i<6; i++) {
        DispPnt[i] = POINT_OFF;
      }
    }else{
      sec>=1 ? DispPnt[1]=POINT_ON : DispPnt[1]=POINT_OFF;
      sec>=60 ? DispPnt[3]=POINT_ON : DispPnt[3]=POINT_OFF;
      sec>=3600 ? DispPnt[5]=POINT_ON : DispPnt[5]=POINT_OFF;
    }
    tm1637_6D.display(DispNum, DispPnt);
  }

}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  pinMode(Status_LED, OUTPUT);
  pinMode(PS4_Button, OUTPUT);
  digitalWrite(Status_LED, LOW);
  digitalWrite(PS4_Button, LOW);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);

  //TM1637 display initialisation
  tm1637_6D.init();
  // You can set the brightness level from 0(darkest) till 7(brightest) or use one
  // of the predefined brightness levels below
  tm1637_6D.set(BRIGHT_TYPICAL);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
}

void loop() {

  // check for a received message
  if (received) {
    // reset flag
    received = false;

    // store as state (redundant but whatever)
    state = myData.f;

    // pull PS4_Button HIGH or LOW, depending on the received state
    digitalWrite(PS4_Button, state);
    digitalWrite(Status_LED, state);

    //check whether button was pressed for longer than 5 seconds on release and start timer if so
    fiveSecondsPressed();

    // detect fast changes in state and reset timer if remote button was pressed fast at least 3 times
    triplePressReset();

    Serial.printf("Changed state: %d\n", state);
  }

  if (timerstart) {
    timediff = millis() - t2;
  }
  showDisplay(timediff);

  delay(10);
}