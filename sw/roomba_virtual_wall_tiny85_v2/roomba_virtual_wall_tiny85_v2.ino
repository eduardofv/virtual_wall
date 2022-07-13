
#include <avr/pgmspace.h>

#include "tiny_IRremote.h"
#include "tiny_IRremoteInt.h"

#define LED_PIN           0  //Arduino pin 0 == ATTiny85 physical pin 5
#define IR_LED_PIN        38 //Arduino pin 4? == ATTiny85 physical pin 3
#define BAT_LEVEL_PIN     2 //"P2", physical
#define BAT_LEVEL_INPUT   1 //Corresponds to PIN_2, physical 7 

//En el ATTiny el indicador aún no funciona como quisiera
//BEGIN GENERATED CODE
//LED levels lookup table generated in `generate_led_levels.py`
#define VMAX              3.3
#define VMIN              2.0
#define MIN_LED           128
#define LED_HIGH_PERIOD   75
#define LED_TOTAL_PERIOD  1000
#define VOLTAGE_SPREAD    5
#define STEPS_PER_PERIOD  5
#define COUNTER_DIVIDER   15
#define VOLTAGE_INDEX(v)  ((v) - 2.0) * 3

const PROGMEM uint8_t LED_LEVELS[] = {0x19,0x33,0x4d,0x66,0x80,0x80,0x26,0x4c,0x73,0x99,0x9a,0x9a,0x3b,0x77,0xb3,0xb3,0xb3,0xb3,0x65,0xcb,0xcd,0xcd,0xcd,0xcd,0xe3,0xe6,0xe6,0xe6,0xe6,0xe6,0xff,0xff,0xff,0xff,0xff,0xff};
//END GENERATED CODE

IRsend irsend;
unsigned int counter = 0;
int last_led_val = 0;

void setup()
{
  //TCCR0B = TCCR0B & 0b11111000 | 0b001;
  pinMode(LED_PIN, OUTPUT);
  pinMode(BAT_LEVEL_PIN, INPUT);
  irsend.enableIROut(IR_LED_PIN);
}

void send_beacon() {
  irsend.mark(1000);
  irsend.space(1000);
}

float read_battery_voltage() {
  int sensorValue = analogRead(BAT_LEVEL_INPUT);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (5.0 / 1023.0);
  return voltage;
}

void update_led_status(float v1) {
  counter += 1;
  int led_val = 0;
  if (counter <= LED_HIGH_PERIOD) {
    /* this is the calculation, but check in generate_led_levels.py
    v1 = constrain(v1, VMIN, VMAX);
    float vpdiff = (VMAX - v1) / VRANGE;
    float level = vpdiff + 0.01;
    float max_led = 255.0 * (1 - vpdiff);
    led_val = min(max_led, max_led * counter / (level * LED_HIGH_PERIOD));
    */

    v1 = constrain(v1, VMIN, VMAX);
    int counter_index = counter / COUNTER_DIVIDER;
    int vol_index = VOLTAGE_INDEX(v1);
    int led_val_index = vol_index * (1 + STEPS_PER_PERIOD) + counter_index;
    led_val = pgm_read_byte_near(led_val_index);
  }

  if(last_led_val != led_val){
    analogWrite(LED_PIN, led_val);
    last_led_val = led_val;
  }
  
  if (counter > LED_TOTAL_PERIOD)
    counter = 0;
}

void loop()
{
  send_beacon();
  float v1 = read_battery_voltage();
  update_led_status(v1);
}
