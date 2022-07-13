"""
Generate values for led level lookup table

This makes getting the value levels for the status led fast enough 
in the ATTiny85 instead of calculating it on each iteration.

The led level is a function of the battery level: both he ramp-up 
time and the final led level are tuned to give a "sensation of 
struggle" to light up the led as the batteries are depleted. Or 
at least that's the intention.

The Nano can do this calculations in real time but on the ATTiny85
it takes too long and inteferes with the IR pattern. So a lookup table
is more convenient.
"""

VMAX = 3.3 #Like two full alkaline bateries
VMIN = 2.0 #Under this value the MT3608 will stop boosting to 5V
LED_HIGH_PERIOD = 75 #"time" (steps in reality) that the led will light
LED_TOTAL_PERIOD = 1000 #total period until next led light up
MIN_LED = 128

VOLTAGE_SPREAD = 5 #discrete values to divide voltage
STEPS_PER_PERIOD = 5 #discrete number of levels for each light up period

led_values = []
n = 0;
for v_step in range(VOLTAGE_SPREAD+1):
    v1 = 0.01 + VMIN + v_step * (VMAX - VMIN) / VOLTAGE_SPREAD
    for step_counter in range(1, STEPS_PER_PERIOD+2):
        counter = step_counter * LED_HIGH_PERIOD // STEPS_PER_PERIOD
        vpdiff = (VMAX - v1) / (VMAX - VMIN)
        level = 0.01 + vpdiff
        max_led = MIN_LED + (255 - MIN_LED) * (1 - vpdiff)
        led_val = min(max_led, max_led * counter / (level * LED_HIGH_PERIOD))
        led_val = int(led_val)
        print(f"n:{n}, v1:{v1:.2f}, step:{counter}, led:{led_val}")
        led_values.append(led_val)
        n += 1

led_values_s = ",".join([f"{v:#04x}" for v in led_values])

#Test, also check implementation on arduino
print()
print("//BEGIN GENERATED CODE")
print("//LED levels lookup table generated in `generate_led_levels.py`")
print(f"#define VMAX              {VMAX}")
print(f"#define VMIN              {VMIN}")
print(f"#define MIN_LED           {MIN_LED}")
print(f"#define LED_HIGH_PERIOD   {LED_HIGH_PERIOD}")
print(f"#define LED_TOTAL_PERIOD  {LED_TOTAL_PERIOD}")
print(f"#define VOLTAGE_SPREAD    {VOLTAGE_SPREAD}")
print(f"#define STEPS_PER_PERIOD  {STEPS_PER_PERIOD}")
print(f"#define COUNTER_DIVIDER   {int(LED_HIGH_PERIOD / STEPS_PER_PERIOD)}")
print(f"#define VOLTAGE_INDEX(v)  ((v) - {VMIN}) * {int(VOLTAGE_SPREAD/(VMAX - VMIN))}")
print()
print(f"const PROGMEM uint8_t LED_LEVELS[] = {{{led_values_s}}};")
print("//END GENERATED CODE")
print()


for v in [0, 2.0, 2.1, 2.3, 2.5, 3.0, 3.2, 3.3, 5.0]:
    vo = v
    v = max(v, VMIN)
    v = min(v, VMAX)
    for c in [0, 1, 10, 50, 75]:#, 100]:
        ci = c // int(LED_HIGH_PERIOD/STEPS_PER_PERIOD)
        vi = int((v - VMIN) * VOLTAGE_SPREAD/(VMAX - VMIN))
        li = vi * (1+STEPS_PER_PERIOD) + ci
        print(f"vo:{vo}, v:{v}, c:{c}, ci:{ci}, vi:{vi}, li:{li}, val:{led_values[li]:#04x}")
