#include <IRremote.h>
#include <ir_Lego_PF_BitStreamEncoder.h>

#define ENABLE_SERIAL_OUTPUT false

#define LED_PIN           5//2 //For tiny Arduino Pin 3 = Physical 2 on ATTiny85
#define IR_LED_PIN        38 //ATTiny85 physical pin 3, D3 on Nano
#define LED_HIGH_PERIOD   100
#define LED_TOTAL_PERIOD  1000
#define VMAX              3.3 //Alkaline bateries
#define VMIN              2.0 //MT3608 min operating voltage

// Initiate IRremote on Pin 3 (Physical)
IRsend irsend;
unsigned int counter = 0;
//bool led_high = true;

void setup()
{
  if (ENABLE_SERIAL_OUTPUT) {
    Serial.begin(9600);
  }

  pinMode(LED_PIN, OUTPUT);
  irsend.enableIROut(IR_LED_PIN);
}

void send_beacon() {
  irsend.mark(1000);
  irsend.space(1000);
}

float read_battery_voltage() {
  int sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (5.0 / 1023.0);
  if (ENABLE_SERIAL_OUTPUT) {
    //Serial.println(voltage);
  }
  return voltage;
}

void update_led_status(float v1) {
  counter += 1;
  if (counter <= LED_HIGH_PERIOD) {
    float led_val = 0;
    v1 = max(v1, VMIN);
    v1 = min(v1, VMAX);
    float vpdiff = (VMAX - v1) / (VMAX - VMIN);
    float level = vpdiff + 0.01;
    float max_led = 255.0 * (1 - vpdiff);
    led_val = min(max_led, max_led * counter / (level * LED_HIGH_PERIOD));
    if(ENABLE_SERIAL_OUTPUT)
      Serial.println(led_val);
    analogWrite(LED_PIN, led_val);
  }
  else {
    analogWrite(LED_PIN, 0);
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
