/*
 * Animate an Adafruit LED strand from an Adafruit Gemma M0.
 */

#include <Adafruit_DotStar.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_FreeTouch.h>

#define STRAND_LEN 20
#define STRAND_PIN 0
#define TOUCH_PIN 1
#define TOUCH_CALIBRATION 500
#define VIBRATION_PIN 2

#define BOOT  1
#define READY 2
#define PULSE 3
#define CHASE 4

Adafruit_DotStar builtin(1, INTERNAL_DS_DATA, INTERNAL_DS_CLK, DOTSTAR_BGR);
// NeoPixel DMA library seems to cause a conflict with digitalRead (used for vibrating)
Adafruit_NeoPixel strand(STRAND_LEN, STRAND_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_FreeTouch touch(A0, OVERSAMPLE_8, RESISTOR_50K, FREQ_MODE_NONE);

boolean fault = false;

uint32_t neoRGB(short color, uint8_t brightness = 255) {
  switch(color) {
    case -1:
      return strand.Color(0, 0, 0);
    case 1:
      return strand.Color(0, brightness, 0);
    case 2:
      return strand.Color(0, 0, brightness);
    default:
      return strand.Color(brightness, 0, 0);
  }
}

uint32_t dotRGB(short color, uint8_t brightness = 25) {
  if (fault) return builtin.Color(255, 0, 0);
  // default to red in case called with color + 1 where color was 2
  switch (color) {
    case 1:
      return builtin.Color(0, brightness, 0);
    case 2:
      return builtin.Color(0, 0, brightness);
    default:
      return builtin.Color(brightness, 0, 0);
  }
}

void neoRGBs(uint32_t rgbs[], short color) {
  rgbs[0] = neoRGB(color, 25);
  rgbs[1] = neoRGB(color, 90);
  rgbs[2] = neoRGB(color, 255);
  rgbs[1] = neoRGB(color, 90);
  rgbs[1] = neoRGB(color, 25);
}

void setup() {
  pinMode(VIBRATION_PIN, INPUT_PULLUP);

  digitalWrite(LED_BUILTIN, LOW);

  builtin.begin();
  if (!touch.begin()) fault = true;
  strand.begin();
  // when using NeoPixel DMA
  //if (!strand.begin()) fault = true;
}

void loop() {
  static unsigned long nextFrameAt = 0,
                       readAfter = 0;
  static short state = BOOT,
               frame = 0,
               color = 0;
  static uint32_t builtinRGB,
                  strandRGB;
  static uint32_t strandRGBs[5];
  static boolean hasTouched = false;

  // read the vibration sensor
  if (millis() > readAfter) {
    if (digitalRead(VIBRATION_PIN) == LOW && state == PULSE) {
      digitalWrite(LED_BUILTIN, HIGH);
      state = CHASE;
      // 2 seconds for spring to stop vibrating (debouncing)
      readAfter = millis() + 2000;
    }
    else {
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

  // read the capacitive sensor
  if (touch.measure() > TOUCH_CALIBRATION) {
    if (!hasTouched) {
      builtinRGB = dotRGB(color + 1, 145);
      hasTouched = true;
    }
  }
  else {
    if (hasTouched) {
      if (++color > 2) color = 0;
      builtinRGB = dotRGB(color);
      neoRGBs(strandRGBs, color);
      strandRGB = neoRGB(color);
      hasTouched = false;
    }
  }

  // animation sequence
  if (millis() > nextFrameAt) {
    switch (state) {
      case BOOT:
        neoRGBs(strandRGBs, color);
        strandRGB = neoRGB(color);
        builtinRGB = builtin.Color(0x9E, 0x42, 0x44);
        // show the boot color for a "short second"
        nextFrameAt = millis() + 800;
        state = READY;
        break;
      case READY:
        builtinRGB = dotRGB(color);
        state = PULSE;
        // continue to pulse
      case PULSE:
        strand.setPixelColor(0, strandRGBs[frame++]);
        if (frame > 4) frame = 0;
        strand.show();
        nextFrameAt = millis() + 100;
        break;
      case CHASE:
        // Draw new pixel
        strand.setPixelColor(frame, strandRGB);
        // Erase pixel a few steps back
        strand.setPixelColor(frame - 4, 0);
        strand.show();
        if (frame++ > STRAND_LEN + 4) {
          frame = 0;
          state = PULSE;
        }
        nextFrameAt = millis() + 25;
        break;
      default:
        fault = true;
        break;
    }
  }

  if (builtinRGB != builtin.getPixelColor(0)) {
    builtin.setPixelColor(0, builtinRGB);
    builtin.show();
  }
}
