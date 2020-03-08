/*
 * Animate an Adafruit LED strand and a sequin from an Adafruit Gemma M0.
 */

#include <Adafruit_DotStar.h>
#include <Adafruit_NeoPixel.h>
#include "sine.h"

#define STRAND_LEN 20
#define STRAND_PIN 0
#define SEQUIN_PIN A1
#define SENSOR_PIN 1

#define BOOT  1
#define PULSE 2
#define CHASE 3

Adafruit_DotStar builtin(1, INTERNAL_DS_DATA, INTERNAL_DS_CLK, DOTSTAR_BGR);
Adafruit_NeoPixel strand(STRAND_LEN, STRAND_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(SENSOR_PIN, INPUT_PULLUP);
  pinMode(SEQUIN_PIN, OUTPUT);

  builtin.begin();
  strand.begin();
}

void loop() {
  static unsigned long frameAt = 0,
                       readAt = 0,
                       blinkAt = 0;
  static short state = BOOT,
               frame = 0;
  static boolean blinking = true;
  static uint32_t strandRGB,
                  builtinRGB,
                  prevBuiltinRGB;

  // sensor reading
  if (state == PULSE
      && millis() > readAt
      && digitalRead(SENSOR_PIN) == LOW) {
    analogWrite(SEQUIN_PIN, 0);
    frame = 0;
    state = CHASE;
    // 200 mseconds debounce spring vibration
    readAt = millis() + 200;
  }

  // animation sequence (state machine)
  if (millis() > frameAt) {
    switch (state) {
      case BOOT:
        strandRGB = strand.Color(0xFF, 0xFF, 0xFF);
        strand.fill(0, 0, STRAND_LEN);
        strand.show();
        builtinRGB = 0;
        prevBuiltinRGB = UINT32_MAX;
        state = PULSE;
        break;
      case PULSE:
        builtinRGB = 0;
        analogWrite(SEQUIN_PIN, sine[frame++]);
        if(frame > SINE_LEN - 1) frame = 0;
        break;
      case CHASE:
        builtinRGB = builtin.Color(0x1B, 0x1B, 0xCD);
        // draw new pixel and erase a few step back
        strand.setPixelColor(frame, strandRGB);
        strand.setPixelColor(frame - 4, 0);
        strand.show();
        if (frame++ > STRAND_LEN + 4) {
          frame = 0;
          state = PULSE;
        }
        break;
      default:
        // should never be here
        builtinRGB = builtin.Color(0x64, 0x00, 0x10);
        break;
    }
    frameAt = millis() + 25;
  }

  // board LEDs report the state
  // help diagnosis of wiring problems with external LEDs
  if (millis() > blinkAt) {
    digitalWrite(LED_BUILTIN, blinking ? LOW : HIGH);
    blinkAt = millis() + (blinking ? 5000 : 200);
    blinking = !blinking;
  }
  if (builtinRGB != prevBuiltinRGB) {
    builtin.setPixelColor(0, builtinRGB);
    builtin.show();
    prevBuiltinRGB = builtinRGB;
  }
}
