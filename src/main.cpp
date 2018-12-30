#include <Arduino.h>
#include <Armothy.h>
#include <params.h>

armothy::Armothy _armothy;

unsigned long _last;
bool led;

// the setup routine runs once when you press reset:
void setup() {
  _armothy.setup();
}

// the loop routine runs over and over again forever:
void loop() {
	_armothy.loop();
}
