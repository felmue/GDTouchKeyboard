/**
 * @file GDTouchKeyboardMac.ino
 * @brief Demonstrates MAC address entry with tactile touch feedback.
 *
 * The keyboard accepts exactly twelve hexadecimal digits and returns the
 * address in the conventional colon-separated format.
 *
 * @copyright Copyright (c) Matthias Prinke. All rights reserved.
 * @license MIT
 */

#include <M5Unified.h>
#include <GDTouchKeyboard.h>

void setup()
{
  auto config = M5.config();
  M5.begin(config);
  Serial.begin(115200);

  GDTK.setAvailableModes(1 << GDTouchKeyboard::KEY_MODE_MAC);
  GDTK.setInputLength(12, 12);
  GDTK.setTouchFeedback(true);
}

void loop()
{
  const String macAddress = GDTK.run(
      "", 0x0ad9, true, &fonts::Font0,
      GDTouchKeyboard::KEY_MODE_MAC, true);

  Serial.print("MAC address: ");
  Serial.println(macAddress);
  delay(1000);
}
