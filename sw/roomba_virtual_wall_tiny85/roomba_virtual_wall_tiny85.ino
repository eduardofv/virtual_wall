/*
  A trivial Arduino sketch to mimic iRobot Virtual Wall for Roomba
Youtube : http://www.youtube.com/mkmeorg Website, Forum and store are at http://mkme.org
Parts needed
Arduino NANO https://amzn.to/2EaMzZI
IR LED https://amzn.to/2QOqUgE
Resistor https://amzn.to/2EhIbsf
  ----------------------------------------------------------------
  Based on information found at:
  http://sites.google.com/site/irobotcreate2/createanirbeacon
  
  Uses "A Multi-Protocol Infrared Remote Library for the Arduino":
  http://www.arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
*/

#include "tiny_IRremote.h"
#include "tiny_IRremoteInt.h"

#define LED_PIN 3 //Arduino Pin 3 = Physical 2
#define LED_HIGH_PERIOD 150
#define LED_TOTAL_PERIOD 1000

// Initiate IRremote on Pin 3 (Physical)
IRsend irsend;
int counter = 0;
bool led_high = false;

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  irsend.enableIROut(38);
}

void loop()
{
//  if(counter % LED_FLASH_CYCLE) led_high = !led_high;
  if(counter > LED_HIGH_PERIOD) led_high = false;
  digitalWrite(LED_PIN, led_high);
 
  irsend.mark(1000);
  irsend.space(1000);

  counter += 1;
  if(counter > LED_TOTAL_PERIOD){
    counter = 0;
    led_high = true;
  }
}
