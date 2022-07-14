
// attiny low power mode http://www.technoblogy.com/show?KX0
//prepare arduino uno as isp programmer https://highlowtech.org/?p=1706
//  - burn the ISP programmer sketch
//attiny programming http://highlowtech.org/?p=1695
//  - install boards
//  - select boards, ATTiny uc
//  - select processor attiny85
//  - set clock at 8 MHz Internal
//    - El primer attiny lo tengo que poner a 8Mhz pero el segundo a 1Mhz ??!!
//  * compile
// ** Use original Arduino UNO
// attiny85 pinout https://nerdytechy.com/attiny85-microcontroller-guide/

#include <avr/pgmspace.h>
#include <avr/sleep.h>

#include "tiny_IRremote.h"
#include "tiny_IRremoteInt.h"

#define LED_PIN           0  //Arduino pin 0 == ATTiny85 physical pin 5 para el prototipo 2
                             //Arduino pin 3 == ATTiny85 physical pin 2 para el prototipo 1
#define IR_LED_PIN        38 //Arduino pin 4? == ATTiny85 physical pin 3
#define BAT_LEVEL_PIN     2 //"P2", physical
#define BAT_LEVEL_INPUT   1 //Corresponds to PIN_2, physical 7 
//#define MAX_TIME_SEC      20 //Test
#define MAX_TIME_SEC      2 * 60 * 60 //Max run time in seconds. Will shutdown after this time (2h)

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

// Utility macro
#define adc_disable() (ADCSRA &= ~(1<<ADEN)) // disable ADC (before power-off)

IRsend irsend;
unsigned int counter = 0;
int last_led_val = 0;

void setup()
{
  //TCCR0B = TCCR0B & 0b11111000 | 0b001;
  pinMode(LED_PIN, OUTPUT);
  pinMode(BAT_LEVEL_PIN, INPUT);
  irsend.enableIROut(IR_LED_PIN);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void send_beacon() {
  irsend.mark(1000);
  irsend.space(1000);
}

void enterSleep (void)
{
  //Sleep mode decreses current from ~40mA to ~14mA in my circuit
  //  voltage booster still uses quite some energy, though
  //  maybe later build a latching circuit 
  
  //Shutdown notification
  for(int i=0; i<50; i++){
    digitalWrite(LED_PIN, 0);
    delay(100);
    digitalWrite(LED_PIN, 1);
    delay(100);
    digitalWrite(LED_PIN, 0);
  }
  
  adc_disable();
  //digitalWrite(LED_PIN, 0);

  sleep_enable();
  sleep_cpu();
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
  if(millis() > MAX_TIME_SEC * 1000)
    enterSleep();
}
