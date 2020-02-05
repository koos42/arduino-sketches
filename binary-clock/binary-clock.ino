const unsigned int outputPins = 0x07FF; // 0b 0000 0111 1111 1111
const unsigned int secondPins = 0x0001; // 0b 0000 0000 0000 0001
const unsigned int minutePins = 0x007E; // 0b 0000 0000 0111 1110
const unsigned int hourPins   = 0x0780; // 0b 0000 0111 1000 0000
const unsigned int buttonInputPin = 11;
const unsigned int tick = 10; // in ms.
const unsigned int totalDelayPerSec = 985; // make lenght of a "second" adjustable.
const unsigned int ticksPerSec = totalDelayPerSec / tick;

// model related
unsigned int seconds;
unsigned int tickCount;
float brightness; // a percent: (0.0 .. 1.0)

// control related
bool buttonPressed; // indicates if button is currently pressed
unsigned int buttonPressTickCount; // seconds at time of button press

// display related
unsigned int outputPinState;

// #define DEBUG

void setup() {  
  #ifdef DEBUG
  Serial.begin(9600);
  #endif

  for (int i = 0; i < 11; i++) {
    pinMode(i, OUTPUT);
  }
  pinMode(buttonInputPin, INPUT);

  seconds = 0;
  outputPinState = 0;
  tickCount = 0;
  brightness = 0.2;

  buttonPressed = false;
  buttonPressTickCount = 0;
}

// The loop is where everything happens, as per usual. Bookkeeping in this context
// means keeping track of certain counters related to the various pieces of state
// being trackded through iterations of the loop (Think Model). Handling input is
// where external inputs are to be used to update the internal state (Think Controller).
// And doDisplay is how that internal state is rendered to the world (Think Viewer).
// The contract here is that doDisplay needs to call the delays that are also
// used as timing for the clock. It will only call the delay for a sum equal
// to that of the "tick" variable, so loop should last as long as "tick"
void loop() {
  handleBookkeeping();
  handleInput();
  doDisplay();
}

void handleBookkeeping() {
  // ensure that tickCount loops, and seconds tick on the second
  if (tickCount >= ticksPerSec) {
    tickCount = 0;
    seconds++;
  }
  
  // ensure this is a 12 hour clock
  if (seconds >= 43200) {
    seconds = 0;
  }

  tickCount++;
}

void handleInput(){
  if (!buttonPressed && digitalRead(buttonInputPin) == HIGH) {
    initiateButtonPress();
  } else if (buttonPressed && digitalRead(buttonInputPin) == HIGH) {
    handleButtonHold();
  } else if (buttonPressed && digitalRead(buttonInputPin) == LOW) {
    handleButtonReleased();
  } else {
    // the button is not being pressed or has not been let go.
  }
}

// don't update the "model", just record that the button press has started
void initiateButtonPress() {
  buttonPressed = true;
  buttonPressTickCount = 0;
}

void handleButtonHold() {
  if (buttonPressTickCount >= 2 * ticksPerSec) { // ongoing long press

    // this is a feature, but also a necessity, because it would require
    // another variable to track if it's already a long click versus a
    // short click.
    // add a minute every second
    seconds += 60;

    // reset buttonPressTickCount so it will "go off" again in a second
    buttonPressTickCount = ticksPerSec;
  } else {
    buttonPressTickCount++;
  }
}

void handleButtonReleased() {
  if (buttonPressTickCount < ticksPerSec) { // end of a short press
    // add .1 to brightness, but limit it to (0.0..1.0)
    brightness = (float)((int)(brightness * 10.0 + 1.0) % 11) / 10.0;
  } else { // end of a long press
    // NO-OP
  }

  buttonPressTickCount = 0;
  buttonPressed = false;
}

void doDisplay() {
  computeOutputPinState();
  
  // use pulse width modulation via delay to adjust brightness
  int onPulseWidth = tick * brightness;
  int offPulseWidth = tick - onPulseWidth;
  
  displayNothingViaPins();
  delay(offPulseWidth);

  displayOutputViaPins();
  delay(onPulseWidth);

  #ifdef DEBUG
  // displayOutputViaSerial();
  #endif
}

void computeOutputPinState() {
  unsigned int secondsMask = computeSecondsMask(seconds);
  unsigned int minutesMask = computeMinutesMask(seconds);
  unsigned int hoursMask = computeHoursMask(seconds);

  outputPinState = outputPins & secondsMask & minutesMask & hoursMask;
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
