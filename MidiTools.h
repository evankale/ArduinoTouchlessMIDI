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

#define MIDI_DEBUG false

//MIDI baud rate
#if MIDI_DEBUG
#define MIDI_SERIAL_RATE 9600
#else
#define MIDI_SERIAL_RATE 31250
#endif

//MIDI defines
#define MIDI_NOTE_ON_CMD 0x90
#define MIDI_NOTE_OFF_CMD 0x80
#define MIDI_CONTROL_CHANGE_CMD 0xB0
#define MIDI_MAX_VELOCITY 127
#define MIDI_MAX_CC_VALUE 127

#define MIDI_GENERAL_PURPOSE_SLIDER1 0x10
#define MIDI_GENERAL_PURPOSE_SLIDER2 0x11
#define MIDI_GENERAL_PURPOSE_SLIDER3 0x12
#define MIDI_GENERAL_PURPOSE_SLIDER4 0x13

void midiControlChange(byte controllerNumber, byte value)
{
#if MIDI_DEBUG
    Serial.print("CC\t");
    Serial.print(controllerNumber);
    Serial.print("\t");
    Serial.println(value);
#else
    Serial.write(MIDI_CONTROL_CHANGE_CMD);
    Serial.write(controllerNumber);
    Serial.write(value);
#endif
}

void midiNoteOn(byte note, byte midiVelocity)
{
  //TODO: Serial.write(NOTE_ON_CMD|channel);
  Serial.write(MIDI_NOTE_ON_CMD);
  Serial.write(note);
  Serial.write(midiVelocity);
}

void midiNoteOff(byte note, byte midiVelocity)
{
  Serial.write(MIDI_NOTE_OFF_CMD);
  Serial.write(note);
  Serial.write(midiVelocity);
}
