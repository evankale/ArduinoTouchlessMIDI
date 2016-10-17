/*
 * Copyright (c) 2016 Evan Kale
 * Media: @EvanKale91
 * Email: EvanKale91@gmail.com
 * Website: www.youtube.com/EvanKale91
 *          www.ISeeDeadPixel.com
 *          www.evankale.blogspot.ca
 *          
 *
 * This file is part of ArduinoTouchlessMIDI.
 *
 * ArduinoTouchlessMIDI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include "MidiTools.h"

// Number of cycles from external counter needed to generate a signal event
#define CYCLES_PER_SIGNAL 2000
#define UNSIGNED_LONG_MAX_VALUE 4294967295
#define NUM_ROLLING_SAMPLES 4
#define SETTLE_TIME 1500000
#define SENSITIVITY 0.25

//Buttons to change sliders (effects)
#define BTN1_PIN 2
#define BTN2_PIN 3
#define BTN3_PIN 4

unsigned long lastSignalTime = 0;
unsigned int signalTimeDelta[NUM_ROLLING_SAMPLES] = {0};
unsigned int currentSignalTimeDeltaIndex = 0;
boolean isSettling = true;

unsigned long rollingDeltaSum = 0;
unsigned long longestDeltaSum = 0;

byte currentCCValue = 0;
unsigned int currentSlider = 1;

// This signal is called whenever OCR1A reaches 0
// (Note: OCR1A is decremented on every external clock cycle)
SIGNAL(TIMER1_COMPA_vect)
{
  unsigned long currentTime = micros();

  //remove oldest signal delta from rolling sum
  rollingDeltaSum -= signalTimeDelta[currentSignalTimeDeltaIndex];

  unsigned int newSignalTimeDelta;
  if (currentTime < lastSignalTime)
  {
    //overflow of micros() has occurred
    newSignalTimeDelta =  (unsigned int)(currentTime + (UNSIGNED_LONG_MAX_VALUE - lastSignalTime));
  }
  else
  {
    newSignalTimeDelta =  (unsigned int)(currentTime - lastSignalTime);
  }

  //add newest signal delta to rolling sum
  rollingDeltaSum += newSignalTimeDelta;

  //replace oldest entry in signal delta buffer
  signalTimeDelta[currentSignalTimeDeltaIndex] = newSignalTimeDelta;

  if (isSettling && currentTime > SETTLE_TIME)
  {
    isSettling = false;
  }

  if (!isSettling && rollingDeltaSum > longestDeltaSum)
  {
    longestDeltaSum = rollingDeltaSum;
  }

  //increment signalTimeDeltaIndex
  currentSignalTimeDeltaIndex++;
  if (currentSignalTimeDeltaIndex == NUM_ROLLING_SAMPLES)
    currentSignalTimeDeltaIndex = 0;

  lastSignalTime = currentTime;

  // Reset OCR1A
  OCR1A += CYCLES_PER_SIGNAL;
}

void setup()
{
  Serial.begin(MIDI_SERIAL_RATE);

  // Set WGM(Waveform Generation Mode) to 0 (Normal)
  TCCR1A = 0b00000000;

  // Set CSS(Clock Speed Selection) to 0b111 (External clock source on T0 pin
  // (ie, pin 5 on UNO). Clock on rising edge.)
  TCCR1B = 0b00000111;

  // Enable timer compare interrupt A (ie, SIGNAL(TIMER1_COMPA_VECT))
  TIMSK1 |= (1 << OCIE1A);

  // Set OCR1A (timer A counter) to 1 to trigger interrupt on next cycle
  OCR1A = 1;

  pinMode(BTN1_PIN, INPUT_PULLUP);
  pinMode(BTN2_PIN, INPUT_PULLUP);
  pinMode(BTN3_PIN, INPUT_PULLUP);
}

void loop()
{
  if (digitalRead(BTN1_PIN) == LOW)
  {
    if (currentSlider != 1)
    {
      currentSlider = 1;
      midiControlChange(MIDI_GENERAL_PURPOSE_SLIDER2, 0);
      midiControlChange(MIDI_GENERAL_PURPOSE_SLIDER3, 0);
    }
  }
  else if (digitalRead(BTN2_PIN) == LOW)
  {
    if (currentSlider != 2)
    {
      currentSlider = 2;
      midiControlChange(MIDI_GENERAL_PURPOSE_SLIDER1, 0);
      midiControlChange(MIDI_GENERAL_PURPOSE_SLIDER3, 0);
    }
  }
  else if (digitalRead(BTN3_PIN) == LOW)
  {
    if (currentSlider != 3)
    {
      currentSlider = 3;
      midiControlChange(MIDI_GENERAL_PURPOSE_SLIDER1, 0);
      midiControlChange(MIDI_GENERAL_PURPOSE_SLIDER2, 0);
    }
  }

  if (!isSettling)
  {
    int sumDifference = (longestDeltaSum - rollingDeltaSum) / 10;

    if (sumDifference > MIDI_MAX_CC_VALUE)
      sumDifference = MIDI_MAX_CC_VALUE;

    if (sumDifference != currentCCValue)
    {
      currentCCValue = sumDifference;
      switch (currentSlider)
      {
        case 1:
          midiControlChange(MIDI_GENERAL_PURPOSE_SLIDER1, sumDifference);
          break;
        case 2:
          midiControlChange(MIDI_GENERAL_PURPOSE_SLIDER2, sumDifference);
          break;
        case 3:
          midiControlChange(MIDI_GENERAL_PURPOSE_SLIDER3, sumDifference);
          break;
      }
    }
  }
}

