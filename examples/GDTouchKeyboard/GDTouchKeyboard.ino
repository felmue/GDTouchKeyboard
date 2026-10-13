#include <M5Unified.h>
#include <GDTouchKeyboard.h>

void setup()
{
  auto config = M5.config();
  // enable serial output
  config.serial_baudrate = 115200;

  M5.begin(config);
}

void loop()
{
  String t = GDTK.run();
  Serial.println(t);
  delay(1000);
}
