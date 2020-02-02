
// setup pin names
const int secondPins[1] = {0};
const int minutePins[6] = {1, 2, 3, 4, 5, 6};
const int hourPins[6] = {7, 8, 9, 10};

void setup() {  
  for (int i = 0; i < 11; i++) {
    pinMode(i, OUTPUT);
  }
}

// the loop function runs over and over again forever
void loop() {
  int seconds = 0;

  // at 12 hours' worth of seconds, allow loop() to complete and restart.
  while (seconds < 43200) {
    seconds += 1;
    printToPins(seconds);
    delay(1000);
  }
}

void printToPins(int seconds) {
  printSecondsToPins(seconds);
  printMinutesToPins(seconds);
  printHoursToPins(seconds);
}

// sets seconds pin high for odd seconds.
void printSecondsToPins(int seconds) {
  if (bitRead(seconds, 0)) {
    digitalWrite(secondPins[0], HIGH);
  } else {
    digitalWrite(secondPins[0], LOW);
  }
}


void printMinutesToPins(int seconds) {
  // count the number of minutes stored in the seconds variable,
  // mod it by one hour's worth of minutes, to get a number in (0..59)
  int minutes = seconds / 60 % 60;

  // Set the output pin in the array of minute pins to high or low
  // depending on the corresponding bit of the minute number.
  for (int i = 0; i < (sizeof(minutePins) / sizeof(minutePins[0])); i++) {
    if (bitRead(minutes, i)) {
      digitalWrite(minutePins[i], HIGH);
    } else {
      digitalWrite(minutePins[i], LOW);
    }
  }
}

void printHoursToPins(int seconds) {
  // count the number of hours stored in the seconds variable,
  // mod it by one half day's worth of hours, to get a number in (0..12)
  int hours = seconds / 60 / 60 % 12;

  // don't display 0 hours, display 12 instead.
  if (hours == 0) {
    hours = 12;
  }

  // Set the output pin in the array of hours pins to high or low
  // depending on the corresponding bit of the hour number.
  for (int i = 0; i < (sizeof(hourPins) / sizeof(hourPins[0])); i++) {
    if (bitRead(hours, i)) {
      digitalWrite(hourPins[i], HIGH);
    } else {
      digitalWrite(hourPins[i], LOW);
    }
  }
}
