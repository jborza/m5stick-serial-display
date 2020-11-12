#include "M5StickC.h"

#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <freertos/task.h>

#define DISPLAY_LINE_LENGTH 240
#define RECEIVE_LINE_BITS 120
#define RECEIVE_LINE_BYTES 25//120/8
#define BAUD_RATE 500000
#define HSYNC 0x48
#define VSYNC 0x56

uint8_t rxBuffer[RECEIVE_LINE_BYTES];
uint8_t rxBufferCopy[RECEIVE_LINE_BYTES];

char displayBuffer[RECEIVE_LINE_BYTES+1];
int row = 0;

#define CENTER 80
#define FONT 1

const int ledPin = 10;

//task that handles the display part
TaskHandle_t displayTask;

//task that handles the receiver part
TaskHandle_t serialTask;


void display_task( void * parameter) {
  for(;;) {
    // black to wait for loopTask to notify this task
    ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
    // copy the received data
    memcpy(rxBufferCopy, rxBuffer, RECEIVE_LINE_BYTES);
    //notify the serial task that it can receive more data
    xTaskNotifyGive(serialTask);
    //clear the display buffer to remove junk from previous transmission
    memset(displayBuffer, 0, RECEIVE_LINE_BYTES);
    memcpy(displayBuffer, rxBufferCopy, RECEIVE_LINE_BYTES);
    M5.Lcd.setCursor(0, row);
    M5.Lcd.print(displayBuffer); 
    row += 8;
    if(row >= 80){
      row = 0;
      M5.Lcd.fillScreen(BLACK);
    }   
  }
}

void serial_task(void * parameter){
  for(;;){
//    size_t xBytesSent;
    //get notified by the display task, or time out on the initial processing
    ulTaskNotifyTake(pdTRUE, ( TickType_t ) 100 );
    int read = Serial.readBytes(rxBuffer, RECEIVE_LINE_BYTES);
    if(read == 0)
    {
      Serial.write(VSYNC);
      return;
    }
    //send notification to displayTask, bringing it out of Blocked state
    xTaskNotifyGive(displayTask);
  }
}

void setup() {
  M5.begin();
  //horizontal rotation
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(1);
  pinMode (ledPin, OUTPUT);


    //setup my core 1 task
  xTaskCreatePinnedToCore(
      serial_task, //task callback
      "SerialTask",// name of the task 
      10000, //task stack size
      NULL,  //task input parameter
      0,     //Priority of the task - 0 = lowest */
      &serialTask, //task handle
      1);  //task core (0 is available, 1 is used by arduino loop()

    //setup my core 0 task
  xTaskCreatePinnedToCore(
      display_task, //task callback
      "DisplayTask",// name of the task 
      10000, //task stack size
      NULL,  //task input parameter
      1,     //Priority of the task - 0 = lowest */
      &displayTask, //task handle
      0);  //task core (0 is available, 1 is used by arduino loop()

  Serial.begin(BAUD_RATE);
  Serial.setTimeout(10000);
}

void loop() {
  digitalWrite (ledPin, LOW);  
  delay(500); 
  digitalWrite (ledPin, HIGH);
  delay(500);  
} 
