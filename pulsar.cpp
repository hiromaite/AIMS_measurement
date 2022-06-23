#include <Arduino.h>
#include <M5Core2.h>

// define pin-name
#define trigger 25

void setup()
{
  // Initialize M5Core2, Don't use M5.begin() cuz it conflicts with BLE initialization.
  Axp.begin();
  M5.Rtc.begin();
  Serial.begin(115200);

  // Initialize the GPIO mode
  pinMode(trigger, OUTPUT);
  // Initialize GPIO outputs
  digitalWrite(trigger, LOW);
}

void loop()
{
  for (int i = 0; i < 100; i++)
  {
    digitalWrite(trigger, HIGH);
    delayMicroseconds(20);
    digitalWrite(trigger, LOW);
    delayMicroseconds(180);
  }
  delay(2000);
}
