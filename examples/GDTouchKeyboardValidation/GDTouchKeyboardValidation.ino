/**
 * @file GDTouchKeyboardValidation.ino
 * @brief Demonstrates configuration entry validation with GDTouchKeyboard.
 *
 * The example presents three editable configuration entries: a text value
 * with a length range, a restricted name, and an eight-character hexadecimal
 * value. The overview is navigated with the Core2 buttons A, B, and C.
 *
 * @copyright Copyright (c) Matthias Prinke. All rights reserved.
 * @license MIT
 */

#ifndef GDTOUCHKEYBOARD_ENABLE_SCREENSHOT
#define GDTOUCHKEYBOARD_ENABLE_SCREENSHOT 0
#endif

#include <M5Unified.h>
#include <GDTouchKeyboard.h>

#if GDTOUCHKEYBOARD_ENABLE_SCREENSHOT
#include <M5SerialPngCapture.h>
#endif

struct ConfigurationEntry
{
  const char *name;
  const char *prompt;
  const char *description;
  uint16_t minimumLength;
  uint16_t maximumLength;
  uint8_t availableModes;
  GDTouchKeyboard::key_mode_t initialMode;
  GDTouchKeyboard::input_validator_t validator;
};

/**
 * @brief Check whether a string contains only letters, '-' or '_'.
 *
 * @param candidate String to validate.
 * @return `true` when every character is allowed, otherwise `false`.
 */
static bool isLettersAndSeparators(const String& candidate)
{
  for (size_t i = 0; i < candidate.length(); ++i)
  {
    const char character = candidate[i];
    if (!((character >= 'a' && character <= 'z') ||
          (character >= 'A' && character <= 'Z') ||
          character == '-' || character == '_'))
    {
      return false;
    }
  }
  return true;
}

/**
 * @brief Check whether a string contains only hexadecimal digits.
 *
 * @param candidate String to validate.
 * @return `true` when every character is hexadecimal, otherwise `false`.
 */
static bool isHex(const String& candidate)
{
  for (size_t i = 0; i < candidate.length(); ++i)
  {
    const char character = candidate[i];
    if (!((character >= '0' && character <= '9') ||
          (character >= 'a' && character <= 'f') ||
          (character >= 'A' && character <= 'F')))
    {
      return false;
    }
  }
  return true;
}

static const ConfigurationEntry configuration[] =
{
  {"Text", "Text (4-12 chars)", "letters, numbers, symbols, 4-12 characters", 4, 12,
   (1 << GDTouchKeyboard::KEY_MODE_LETTER) |
   (1 << GDTouchKeyboard::KEY_MODE_NUMBER),
   GDTouchKeyboard::KEY_MODE_LETTER, nullptr},
  {"Name", "Name (a-z, A-Z, -/_, max 32 chars)", "a-z, A-Z, '-' or '_'", 1, 32,
    (1 << GDTouchKeyboard::KEY_MODE_LETTER) |
    (1 << GDTouchKeyboard::KEY_MODE_NUMBER),
   GDTouchKeyboard::KEY_MODE_LETTER, isLettersAndSeparators},
  {"Hex value", "Hex value (8 hex digits)", "exactly 8 digits: 0-9, A-F", 8, 8,
   1 << GDTouchKeyboard::KEY_MODE_HEX,
   GDTouchKeyboard::KEY_MODE_HEX, isHex},
};

static const size_t configurationCount = sizeof(configuration) / sizeof(configuration[0]);
static String configurationValues[configurationCount];
static const bool configurationTouchFeedback = true;

#if GDTOUCHKEYBOARD_ENABLE_SCREENSHOT
static M5SerialPngCapture screenshotCapture(Serial);

static void sendScreenshot()
{
  if (!screenshotCapture.captureAndSend(M5.Display))
  {
    Serial.println("SCREENSHOT_ERROR");
  }
}
#endif

/**
 * @brief Provide optional tactile feedback for an overview button action.
 */
static void provideConfigurationTouchFeedback()
{
  if (configurationTouchFeedback)
  {
    M5.Power.setVibration(180);
    delay(80);
    M5.Power.setVibration(0);
  }
}

/**
 * @brief Draw the configuration entries and physical-button legend.
 *
 * @param selectedEntry Index of the entry to highlight.
 */
static void drawConfigurationOverview(size_t selectedEntry)
{
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);

  for (size_t i = 0; i < configurationCount; ++i)
  {
    const int y = 2 + (i * 54);
    const bool selected = i == selectedEntry;
    const uint16_t background = selected ? TFT_WHITE : TFT_BLACK;
    const uint16_t foreground = selected ? TFT_BLACK : TFT_WHITE;

    M5.Display.fillRect(0, y, M5.Display.width(), 50, background);

    M5.Display.setFont(&fonts::Font4);
    M5.Display.setTextColor(foreground, background);
    M5.Display.setTextDatum(TL_DATUM);
    M5.Display.drawString(configuration[i].name, 8, y + 1);

    String value = configurationValues[i];
    if (value.length() == 0)
    {
      value = "<empty>";
    }
    const int nameWidth = M5.Display.textWidth(configuration[i].name);
    const int maximumValueWidth = M5.Display.width() - nameWidth - 24;
    bool valueTruncated = false;
    while (value.length() > 0 && M5.Display.textWidth(value) > maximumValueWidth)
    {
      value = value.substring(1);
      valueTruncated = true;
    }
    if (valueTruncated && value.length() > 3)
    {
      value = "..." + value.substring(3);
    }

    M5.Display.setTextDatum(TR_DATUM);
    M5.Display.drawString(value, M5.Display.width() - 8, y + 1);

    M5.Display.setFont(&fonts::Font2);
    M5.Display.setTextDatum(TL_DATUM);
    M5.Display.drawString(configuration[i].description, 10, y + 30);
  }

  const int labelY = M5.Display.height() - 38;
  const int arrowBaseY = M5.Display.height() - 20;
  const int buttonWidth = M5.Display.width() / 3;
  const char *buttonLabels[] = {"  Prev  ", "  Edit  ", "  Next  "};
  M5.Display.setFont(&fonts::Font2);
  M5.Display.setTextDatum(MC_DATUM);
  M5.Display.setTextColor(TFT_BLACK, TFT_WHITE);
  for (int i = 0; i < 3; ++i)
  {
    const int x = i * buttonWidth;
    const int width = i == 2 ? M5.Display.width() - x : buttonWidth;
    const int centerX = x + (width / 2);
    const int labelWidth = M5.Display.textWidth(buttonLabels[i]) + 4;
    M5.Display.fillRect(centerX - (labelWidth / 2), labelY - 12,
                        labelWidth, 24, TFT_WHITE);
    M5.Display.drawString(buttonLabels[i], centerX, labelY);
    const int labelTextWidth = static_cast<int>(M5.Display.textWidth(buttonLabels[i]));
    const int arrowHalfWidth = max(12, labelTextWidth / 2);
    M5.Display.fillTriangle(centerX, M5.Display.height() - 1,
                            centerX - arrowHalfWidth, arrowBaseY,
                            centerX + arrowHalfWidth, arrowBaseY,
                            TFT_WHITE);
  }
}

/**
 * @brief Display the overview and wait for a navigation action.
 *
 * @param selectedEntry Initially selected entry.
 * @return Index of the entry selected for editing.
 */
static size_t showConfigurationOverview(size_t selectedEntry)
{
  drawConfigurationOverview(selectedEntry);
  while (true)
  {
    M5.update();
#if GDTOUCHKEYBOARD_ENABLE_SCREENSHOT
    GDTK.processScreenshotRequest();
#endif
    if (M5.BtnA.wasClicked())
    {
      provideConfigurationTouchFeedback();
      selectedEntry = selectedEntry == 0 ? configurationCount - 1 : selectedEntry - 1;
      drawConfigurationOverview(selectedEntry);
    }
    else if (M5.BtnC.wasClicked())
    {
      provideConfigurationTouchFeedback();
      selectedEntry = (selectedEntry + 1) % configurationCount;
      drawConfigurationOverview(selectedEntry);
    }
    else if (M5.BtnB.wasClicked())
    {
      provideConfigurationTouchFeedback();
      while (M5.BtnB.isPressed())
      {
        M5.update();
      }
      return selectedEntry;
    }
  }
}

/**
 * @brief Edit one configuration entry using its validation rules.
 *
 * @param entryIndex Index of the entry to edit.
 * @return The value accepted by the keyboard.
 */
static String readConfigurationEntry(size_t entryIndex)
{
  const ConfigurationEntry& entry = configuration[entryIndex];
  Serial.print("Enter ");
  Serial.print(entry.name);
  Serial.println(":");

  GDTK.setTouchFeedback(true);
  GDTK.setAvailableModes(entry.availableModes);
  GDTK.setInputLength(entry.minimumLength, entry.maximumLength);
  GDTK.setInputValidator(entry.validator);
  return GDTK.run(configurationValues[entryIndex], 0x0ad9,
                 true, &fonts::Font0, entry.initialMode, true,
                 entry.prompt);
}

/**
 * @brief Initialize the M5Stack Core2 hardware and serial output.
 */
void setup()
{
  M5.begin();
  Serial.begin(115200);
#if GDTOUCHKEYBOARD_ENABLE_SCREENSHOT
  GDTK.setScreenshotHandler(sendScreenshot);
#endif
}

/**
 * @brief Navigate the overview and edit configuration entries indefinitely.
 */
void loop()
{
  size_t selectedEntry = 0;
  while (true)
  {
    const size_t entry = showConfigurationOverview(selectedEntry);
    const String value = readConfigurationEntry(entry);
    configurationValues[entry] = value;
    Serial.print(configuration[entry].name);
    Serial.print(" = ");
    Serial.println(value);

    selectedEntry = entry;
  }
}