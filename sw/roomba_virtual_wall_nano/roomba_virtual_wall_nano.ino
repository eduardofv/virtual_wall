#include <IRremote.h>
#include <ir_Lego_PF_BitStreamEncoder.h>

#define LED_PIN           2 //For tiny Arduino Pin 3 = Physical 2 on ATTiny85
#define IR_LED_PIN        38 //ATTiny85 physical pin 3, D3 on Nano
#define LED_HIGH_PERIOD   75
#define LED_TOTAL_PERIOD  1000

#define ENABLE_SERIAL_OUTPUT true

// Initiate IRremote on Pin 3 (Physical)
IRsend irsend;
int counter = 0;
bool led_high = true;

void setup()
{
  if(ENABLE_SERIAL_OUTPUT){
    Serial.begin(9600);
  }
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  irsend.enableIROut(IR_LED_PIN);
}

void send_beacon(){
  irsend.mark(1000);
  irsend.space(1000);
}

void read_battery_voltage(){
  int sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (5.0 / 1023.0);
  // print out the value you read:
  if(ENABLE_SERIAL_OUTPUT){
    Serial.println(voltage); 
  } 
}

void update_led_status(){
  if(counter > LED_HIGH_PERIOD) led_high = false;
  digitalWrite(LED_PIN, led_high);
 
  counter += 1;
  if(counter > LED_TOTAL_PERIOD){
    counter = 0;
    led_high = true;
  }
}


void loop()
{
  send_beacon();
  read_battery_voltage();
  update_led_status();
  
}
