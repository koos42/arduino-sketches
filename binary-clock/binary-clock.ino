const unsigned int outputPins = 0x07FF; // 0b 0000 0111 1111 1111
const unsigned int secondPins = 0x0001; // 0b 0000 0000 0000 0001
const unsigned int minutePins = 0x007E; // 0b 0000 0000 0111 1110
const unsigned int hourPins   = 0x0780; // 0b 0000 0111 1000 0000
const unsigned int tick = 10; // in ms.
const unsigned int ticksPerSec = 1000 / tick;

unsigned int seconds;
unsigned int tickCount;
float brightness; // a percent

unsigned int outputPinState;

// #define DEBUG

void setup() {  
  #ifdef DEBUG
  Serial.begin(9600);
  #endif

  for (int i = 0; i < 11; i++) {
    pinMode(i, OUTPUT);
  }

  seconds = 0;
  outputPinState = 0;
  tickCount = 0;
  brightness = 0.2;
}

void loop() {
  // ensure that tickCount loops, and seconds tick on the second
  if (tickCount >= ticksPerSec) {
    tickCount = 0;
    seconds++;
  }
  
  // ensure this is a 12 hour clock
  if (seconds >= 43200) {
    seconds = 0;
  }

  outputPinState = setOutputPinState(seconds);
  setDisplay();
  
  tickCount++;
}

unsigned int setOutputPinState(int seconds) {
  unsigned int secondsMask = computeSecondsMask(seconds);
  unsigned int minutesMask = computeMinutesMask(seconds);
  unsigned int hoursMask = computeHoursMask(seconds);

  return outputPins & secondsMask & minutesMask & hoursMask;
}

unsigned int computeSecondsMask(int seconds) {
  unsigned int mask = seconds & secondPins;

  // return seconds, with 1's set for all other bits of the mask
  return mask | ~secondPins;
}

unsigned int computeMinutesMask(int seconds) {
  // count the number of minutes stored in the seconds variable,
  // mod it by one hour's worth of minutes, to get a number in (0..59)
  int minutes = seconds / 60 % 60;
  
  // shift minutes (0..59) one spot left to fit within the minute pins section
  unsigned int mask = minutes << 1;

  // return minutes, now shifted, with 1's set for all other bits of the mask
  return mask | ~minutePins;
}

unsigned int computeHoursMask(int seconds) {
  // count the number of hours stored in the seconds variable,
  // mod it by one half day's worth of hours, to get a number in (0..12)
  int hours = seconds / 60 / 60 % 12;

  // don't display 0 hours, display 12 instead.
  if (hours == 0) {
    hours = 12;
  }
  
  // shift hours (1..12) 7 spots left to fit within the hours pins section
  unsigned int mask = hours << 7;

  // return hours, now shifted, with 1's set for all other bits of the mask
  return mask | ~hourPins;
}

void setDisplay() {
  // use pulse width modulation via delay to adjust brightness
  int onPulseWidth = tick * brightness;
  int offPulseWidth = tick - onPulseWidth;
  
  displayNothingViaPins();
  delay(offPulseWidth);

  displayOutputViaPins();
  delay(onPulseWidth);

  #ifdef DEBUG
  displayOutputViaSerial();
  #endif
}

void displayOutputViaPins() {
  for (int i = 0; i < 11; i++) {
    if (bitRead(outputPinState, i)) {
      digitalWrite(i, HIGH);
    } else {
      digitalWrite(i, LOW);
    }
  }
}

void displayNothingViaPins() {
  for (int i = 0; i < 11; i++) {
    digitalWrite(i, LOW);
  }
}


#ifdef DEBUG
void displayOutputViaSerial() {
  Serial.print("pins: 0  1  2  3  4  5  6  7  8  9  10\n");
  Serial.print("      ");
  
  for (int i = 0; i < 11; i++) {
    if (bitRead(outputPinState, i)) {
      Serial.print("1  ");
    } else {
      Serial.print("0  ");
    }
  }
  Serial.print("\n");
}
#endif
