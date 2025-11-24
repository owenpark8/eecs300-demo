/*
 * Example Program that counts the number of times the 
 * boot button is pressed and prints it to a server
 * 
 */
#include "WirelessCommunication.h"
#include "sharedVariable.h"

#define BUTTON_PIN 0//boot button

uint32_t is_pressed();
void update_button_count();

volatile uint32_t count = 0;
volatile shared_uint32 x;

void setup()
{
  pinMode(BUTTON_PIN, INPUT);
  Serial.begin(115200);
  init_wifi_task();
  INIT_SHARED_VARIABLE(x, count);//init shared variable used to tranfer info to WiFi core
}

void loop()
{           
  //check if Boot button has been pressed and update values if needed
  if(is_pressed())
  {
    ++count;
    update_button_count();//update shared variable x (shared with WiFi task)
  }
  Serial.println(count);
  delay(10);
}

uint32_t is_pressed()
{
  if(!digitalRead(BUTTON_PIN))
  {
    delay(10);//software debouncing
    if(!digitalRead(BUTTON_PIN))
    {
      while(!digitalRead(BUTTON_PIN));//make sure button is depressed
      return 1;
    }
  }
  return 0;
}


//example code that updates a shared variable (which is printed to server)
//under the hood, this implementation uses a semaphore to arbitrate access to x.value
void update_button_count()
{
  //minimized time spend holding semaphore
  LOCK_SHARED_VARIABLE(x);
  x.value = count;
  UNLOCK_SHARED_VARIABLE(x);   
}
