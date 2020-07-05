# MagicHat

Arduino-based LED animation to sew in a pointy "magician hat."

Sew the sequin in front of the hat, hide the strand, Gemma and battery inside the hat.

## Hardware

- A magician hat
- [Gemma M0](https://www.adafruit.com/product/3501)
- [Vibration sensor switch](https://www.adafruit.com/product/1766)
- [LED Sequin](https://www.adafruit.com/product/1758)
- [NeoPixel LED strand](https://www.adafruit.com/product/3631)
- [Coin cell battery holder](https://www.adafruit.com/product/783)
- [5V Step-up/step-down voltage regulator](https://www.pololu.com/product/2119) (optional)
- PN2222 transistor (optional)
- JST SM 3 poles cable
- Alligator clips and M3 nuts and bolts (prototype)
- Solder (final wiring)

## Wiring

### Gemma

- D0: strand data
- D1: vibration sensor
- D2: PN2222 base
- Vout: regulator VIN
- GND : strand-, sequin-, regulator GND and vibration sensor

#### Soldering thing

You would think that it will be easier to solder to the strand data to D1 but it is
not PWM capable.

### Regulator

Regulates 5V to the strand (optimal for NeoPixels brightness).

- VIN: Gemma Vout
- GND: Gemma GND
- VOUT: strand+

#### Optional PN2222

For the sequin to light up more brightly, use a PN2222 transistor.
Wire the base to the D2 (instead of sequin+), the emitter to Gemma GND and the
collecter to sequin-.
Wire sequin+ to Gemma Vout.

## Sine Wave

The sine wave is generate by `sine.py`. It pulses at low power twice, followed
by one high power.

Reduce/increase sampling to speed the animation up/down.
