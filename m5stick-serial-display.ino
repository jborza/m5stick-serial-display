#include "M5StickC.h"

#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#define DISPLAY_LINE_LENGTH 240
#define RECEIVE_LINE_BITS 120
#define RECEIVE_LINE_BYTES 25//120/8
#define BAUD_RATE 9600
#define HSYNC 0x48
#define VSYNC 0x56

uint8_t rxBuffer[RECEIVE_LINE_BYTES];
uint8_t rxBufferCopy[RECEIVE_LINE_BYTES];

char displayBuffer[RECEIVE_LINE_BYTES+1];
int row = 0;

#define CENTER 80
#define FONT 1

const int ledPin = 10;

TaskHandle_t displayTask;
QueueHandle_t vidQueue;
QueueHandle_t doneQueue;

#define QUEUE_DELAY ( TickType_t ) 100

void display_task( void * parameter) {
  int *queueData = NULL;
  //initial notify to UART task (loop) that we can receive data
  xQueueSend(doneQueue, &queueData, QUEUE_DELAY);
  for(;;) {
    if(xQueueReceive(vidQueue, &queueData, QUEUE_DELAY) != pdPASS){
      continue;
    }
    memcpy(displayBuffer, rxBufferCopy, RECEIVE_LINE_BYTES);
    //clear the receive buffer
    memset(rxBufferCopy, 0, RECEIVE_LINE_BYTES);
    //displayBuffer[RECEIVE_LINE_BYTES] = 0;
    M5.Lcd.setCursor(0, row);
    M5.Lcd.print(displayBuffer); 
    blink();
    //we need to notify the UART task that we can receive data
    xQueueSend(doneQueue, &queueData, QUEUE_DELAY);  
  }
}

void setup() {
M5.begin();
  //horizontal rotation
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(1);
  pinMode (ledPin, OUTPUT);

  //render queue
  vidQueue = xQueueCreate(1, sizeof(uint32_t *));
  //done queue
  doneQueue = xQueueCreate(1, sizeof(uint32_t *));
  
  //setup my core 0 task
  xTaskCreatePinnedToCore(
      display_task, //task callback
      "DisplayTask",// name of the task 
      10000, //task stack size
      NULL,  //task input parameter
      0,     //Priority of the task - 0 = lowest */
      &displayTask, //task handle
      0);  //task core (0 is available, 1 is used by arduino loop()
  
  Serial.begin(BAUD_RATE);
  Serial.setTimeout(5000);
}

void blink(){
  digitalWrite (ledPin, LOW);  // turn on the LED
  delay(5); // wait for half a second or 500 milliseconds
  digitalWrite (ledPin, HIGH); // turn off the LED
}

int temp = 0;

void loop() {
  int *queueData = &temp;
  int *doneQueueData;

  //get notified by the display task
  xQueueReceive(doneQueue, queueData, QUEUE_DELAY);
  int read = Serial.readBytes(rxBuffer, RECEIVE_LINE_BYTES);
  if(read == 0)
  {
    Serial.write(VSYNC);
    return;
  }
  memcpy(rxBufferCopy, rxBuffer, RECEIVE_LINE_BYTES);
  xQueueSend(vidQueue, queueData, QUEUE_DELAY); 
  
  row += 8;
  if(row >= 80){
    row = 0;
    M5.Lcd.fillScreen(BLACK);
  }
} 
