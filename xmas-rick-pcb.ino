#include <Adafruit_NeoPixel.h>
#include <avr/sleep.h>

// MAKE SURE TO BURN BOOTLOADER - OTHERWISE LEDs JUST GLOW WHITE

// disable ADC (before power-off)
#define adc_disable() (ADCSRA &= ~(1<<ADEN));

// Number of LEDs
#define numLeds 6

// GPIO pin to LED data in
#define pin 4

// 2n7000 MOSFET gate pin
#define gate 3

// Define neopixels
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numLeds, pin, NEO_GRBW + NEO_KHZ800);

float redStates[numLeds];
float blueStates[numLeds];
float greenStates[numLeds];
float whiteStates[numLeds];
float fadeRate = .985;
int twinkle = 0;

void setup() {

  // Clear pixels
  strip.begin();
  strip.clear();
  strip.show();

  // Set MOSFET gate pin and turn it on
  pinMode(gate, OUTPUT);
  
  // Disable ADC, uses ~320uA passively
  adc_disable();

  //Set unused pins to low, saves power
  for (int i = 0; i <= 2; i++)
    {
    pinMode (i, OUTPUT);
    digitalWrite (i, LOW);
    }
  
  // Specify power down sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  
  // Set LED states to 0
  for(uint16_t l = 0; l < numLeds; l++) {
    redStates[l] = 0;
    greenStates[l] = 0;
    blueStates[l] = 0;
    whiteStates[l] = 0;
  }
}

void enterSleep(void)
{
  sleep_enable();
  sleep_cpu();
}

void updateLeds(){
  for(uint16_t l = 0; l < numLeds; l++) {
    if (redStates[l] > 1 || greenStates[l] > 1 || blueStates[l] > 1 || whiteStates[l] > 1) {
      strip.setPixelColor(l, redStates[l], greenStates[l], blueStates[l], whiteStates[l]);
      
      if (redStates[l] > 1) {
        redStates[l] = redStates[l] * fadeRate;
      } else {
        redStates[l] = 0;
      }
      
      if (greenStates[l] > 1) {
        greenStates[l] = greenStates[l] * fadeRate;
      } else {
        greenStates[l] = 0;
      }
      
      if (blueStates[l] > 1) {
        blueStates[l] = blueStates[l] * fadeRate;
      } else {
        blueStates[l] = 0;
      }
  
      if (whiteStates[l] > 1) {
        whiteStates[l] = whiteStates[l] * fadeRate;
      } else {
        whiteStates[l] = 0;
      }
      
    } else {
      strip.setPixelColor(l, 0, 0, 0, 0);
    }
  }
  strip.show();
  delay(5);
}

void loop () {

  digitalWrite(gate, HIGH);
  
  while (twinkle <= 100){
    if (random(20) == 1) {
      uint16_t i = random(numLeds);
      if (redStates[i] < 1 && greenStates[i] < 1 && blueStates[i] < 1 && whiteStates[i] < 1) {
        redStates[i] = random(200);
        greenStates[i] = random(200);
        blueStates[i] = random(200);
        whiteStates[i] = random(10);
        twinkle++;
      }
    }
    updateLeds();
  }
  // Blank out Pixels
  for(uint16_t l = 0; l < numLeds; l++){
    strip.setPixelColor(l, 0, 0, 0, 0);
    strip.clear();
    strip.show();
  }
  // Turn mosfet off to kill power to led chips
  delay(20);
  digitalWrite(gate, LOW);
  
  // Set Neopixel data pin to high impedence "INPUT" mode.
  // This is destructive. You must reset the board to re-initialize the data pin to use neopixels
  // Brings idle consumption of Neopixels to 0
  pinMode(pin, INPUT);
  enterSleep(); 
}
