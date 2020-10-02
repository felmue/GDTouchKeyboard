#include <M5Core2.h>
#include <GDTouchKeyboard.h>

void setup()
{
  M5.begin();
}

void loop()
{
  String t = GDTK.run();
  Serial.println(t);
  delay(1000);
}
