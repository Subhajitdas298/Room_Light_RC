/*
  Room Light RC
  Copyright (c) 2016 Subhajit Das

  Licence Disclaimer:

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <IRremote.h>        // necessary for remote data decoding
#include <FRONTECH_TV_Remote.h>  // remote controller

/*
   ::Variables::
  OUTPin[] and onStatus[] used together to define their respective value for a single load.
  They represent different properties of same output. Their index number must be same when accessing them.
  OUTPin[] contains the output pin numbers on the microcontroller / arduino.
  onStatus[] represents their on status, true(1) for on and false(0) for off. Initially 0, as the all off.

   ::Functions::
  void setup(), does initial setup (compulsory).
  void loop(), loop through iterations.
  void setOutputs(), sets outputs HIGH or LOW depending on onStatus[].
  boolean getOUTAny(), returns true if any output is on.
  void checkRemote(long remoteValue), checks for remote inputs, takes action on onStatus[] accordingly.

    When user presses a button on the remote, the data is received by IR receiver.
    Then it is decoded by decoder library. Based on received data, output is switched.
    For 1-6 output is directly switched.
    For power button, current status stored and if any load is on they turned off, or stored value is mimicked in output.
*/

const byte OUTPin[] = {  // output OUT pin no
  3, 4, 5, 6, 7, 8
};

const byte noOfOutputs = sizeof(OUTPin);  // number of OUTPUTs
boolean onStatus[noOfOutputs];  // states if OUT is ON (1) or OFF (0)
boolean onStatusTemp[noOfOutputs];  // temporary onStatus
const byte Indicator = 9;  // indicator blinks when remote data recieves

byte i; // used for iteration control in loops

const byte RECV_PIN = 13;  // recive pin for IR

IRrecv irrecv(RECV_PIN);  // calling constructor of IRrecv
decode_results results;  // decoded input results stored in results object

boolean stateChanged = false;

void setup()
{
  // defining all output pins as OUTPUT
  for (i = 0; i < noOfOutputs; i++) {
    pinMode(OUTPin[i], OUTPUT);
  }
  pinMode(Indicator, OUTPUT);
  irrecv.enableIRIn(); // Start the receiver
  // setting all OUTPUTs to off initially
  for (i = 0; i < noOfOutputs; i++) {
    onStatus[i] = false;
  }
}

void loop() {
  if (irrecv.decode(&results)) {
    long recv = results.value;
    irrecv.resume(); // Receive the next value
    stateChanged = true;
    // checks for Remote control inputs
    checkRemote(recv);
  }
  if (stateChanged) {
    // switches the couputs accordingly
    setOutputs();
  }
  delay(50);
}

// setting all OUT pins according to their respective onStatus
void setOutputs() {
  // blinking indicator
  digitalWrite(Indicator, HIGH);
  delay(300);
  digitalWrite(Indicator, LOW);
  for (i = 0; i < noOfOutputs; i++) {
    digitalWrite(OUTPin[i], onStatus[i]);
  }
}

// returns true if any output is on
boolean getOUTAny() {
  for (i = 0; i < noOfOutputs; i++) {
    if (onStatus[i] == true) {
      return true;
    }
  }
  return false;
}

// checks remote data and sets onStatus
void checkRemote(long remoteValue) {
  switch (remoteValue) {
    case NUM_1 :
      onStatus[0] = !onStatus[0];
      break;
    case NUM_2 :
      onStatus[1] = !onStatus[1];
      break;
    case NUM_3 :
      onStatus[2] = !onStatus[2];
      break;
    case NUM_4 :
      onStatus[3] = !onStatus[3];
      break;
    case NUM_5 :
      onStatus[4] = !onStatus[4];
      break;
    case NUM_6 :
      onStatus[5] = !onStatus[5];
      break;
    case POWER :
      if (getOUTAny()) {
        // setting all OUT pins to LOW
        for (i = 0; i < noOfOutputs; i++) {  // storing current onStatus to temp location
          onStatusTemp[i] = onStatus[i];
          onStatus[i] = false;
        }
      }
      else { //setting all OUT pins to HIGH
        for (i = 0; i < noOfOutputs; i++) {
          // retriving stored onStatus
          onStatus[i] = onStatusTemp[i];
        }
      }
      break;
    default:
      stateChanged = false;
  }
}

