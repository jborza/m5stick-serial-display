#include "M5StickC.h"

RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;


#define DISPLAY_LINE_LENGTH 240
#define RECEIVE_LINE_BITS 120
#define RECEIVE_LINE_BYTES 25//120/8
#define BAUD_RATE 500000
#define HSYNC 0x48
#define VSYNC 0x56

uint8_t rxBuffer[RECEIVE_LINE_BYTES];


#define CENTER 80
#define FONT 1

const int ledPin = 21;

void setup() {
M5.begin();
//horizontal rotation
M5.Lcd.setRotation(3);
M5.Lcd.fillScreen(BLACK);
M5.Lcd.setTextSize(1);
pinMode (ledPin, OUTPUT);
Serial.begin(9600);
}

void blink(){
  digitalWrite (ledPin, HIGH);  // turn on the LED
  delay(5); // wait for half a second or 500 milliseconds
  digitalWrite (ledPin, LOW); // turn off the LED
  //delay(500); // wait for half a second or 500 milliseconds 
}

char displayBuffer[RECEIVE_LINE_BYTES+1];
int row = 0;

void loop() {
  int read = Serial.readBytes(rxBuffer, RECEIVE_LINE_BYTES);
  if(read == 0)
  {
    Serial.write(VSYNC);
    return;
  }
  //clear the receive buffer
  memcpy(displayBuffer, rxBuffer, RECEIVE_LINE_BYTES);
  memset(rxBuffer, 0, RECEIVE_LINE_BYTES);

  //displayBuffer[RECEIVE_LINE_BYTES] = 0;
  M5.Lcd.setCursor(0, row);
  M5.Lcd.print(displayBuffer); 
  blink();
  row += 8;
  if(row >= 80)
    row = 0;
} 
