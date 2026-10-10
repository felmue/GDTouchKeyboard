// Copyright (c) GWENDESIGN. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "GDTouchKeyboard.h"

GDTouchKeyboard::GDTouchKeyboard()
{
}

GDTouchKeyboard::~GDTouchKeyboard()
{
}

String GDTouchKeyboard::run(String text, uint16_t setColourIn,
                            bool getIsEditable,
                            const lgfx::v1::IFont* fontIn,
                            key_mode_t modeIn,
                            bool preserveMacSeparators,
                            String prompt)
{
  isEditable = getIsEditable;
  font = fontIn;
  _preserve_mac_separators = preserveMacSeparators;
  if (setColourIn != COLOR_SCHEME_UNCHANGED)
  {
    _color_scheme.keyBorder = setColourIn;
    _color_scheme.cursor = setColourIn;
  }
  _initKeyboard(text);
  setMode(modeIn);
  if (_key_mode == KEY_MODE_MAC)
  {
    _input_text = _stripMacSeparators(_input_text);
  }
  promptText = _input_text;
  _prompt_text = prompt;
  _drawKeyboard();
  _updateInputText();
  while(_keyboard_done == false)
  {
    M5.update();
    processScreenshotRequest();
    _processInput();
    if (_vibration_stop_at != 0 &&
        static_cast<int32_t>(millis() - _vibration_stop_at) >= 0)
    {
      M5.Power.setVibration(0);
      _vibration_stop_at = 0;
    }

    // Blinking cursor
    if(millis() > _cursor_last)
    {
      _cursor_last = millis() + 500;
      _cursor_state = !_cursor_state;
      _updateInputText();
    }
  }
  while(M5.BtnB.isPressed())
  {
    M5.update();
  }
  M5.Power.setVibration(0);
  _vibration_stop_at = 0;
  _deinitKeyboard();
  if (_key_mode == KEY_MODE_MAC && _preserve_mac_separators)
  {
    return _formatMacInput(_input_text);
  }
  return _input_text;
}

void GDTouchKeyboard::setColorScheme(const color_scheme_t& scheme)
{
  _color_scheme = scheme;
}

const GDTouchKeyboard::color_scheme_t& GDTouchKeyboard::getColorScheme(void) const
{
  return _color_scheme;
}

void GDTouchKeyboard::setMode(key_mode_t modeIn)
{
  if ((_available_modes & (1 << modeIn)) == 0)
  {
    for (uint8_t mode = KEY_MODE_LETTER; mode <= KEY_MODE_MAC; mode++)
    {
      if ((_available_modes & (1 << mode)) != 0)
      {
        modeIn = static_cast<key_mode_t>(mode);
        break;
      }
    }
  }
  _key_mode = modeIn;
  _shift_mode = false;
}

void GDTouchKeyboard::setAvailableModes(uint8_t modeMask)
{
  _available_modes = modeMask & ((1 << (KEY_MODE_MAC + 1)) - 1);
  if (_available_modes == 0)
  {
    _available_modes = (1 << KEY_MODE_LETTER) |
                       (1 << KEY_MODE_NUMBER) |
                       (1 << KEY_MODE_HEX) |
                       (1 << KEY_MODE_MAC);
  }
  setMode(_key_mode);
}

void GDTouchKeyboard::setInputLength(uint16_t minLength, uint16_t maxLength)
{
  _minimum_length = minLength;
  _maximum_length = maxLength;
  if (_maximum_length != 0 && _minimum_length > _maximum_length)
  {
    _minimum_length = _maximum_length;
  }
}

void GDTouchKeyboard::setInputValidator(input_validator_t validator)
{
  _input_validator = validator;
}

void GDTouchKeyboard::setTouchFeedback(bool enabled)
{
  _touch_feedback = enabled;
  if (!enabled)
  {
    M5.Power.setVibration(0);
    _vibration_stop_at = 0;
  }
}

void GDTouchKeyboard::setScreenshotHandler(screenshot_handler_t handler)
{
  _screenshot_handler = handler;
}

void GDTouchKeyboard::processScreenshotRequest()
{
  while (Serial.available() > 0)
  {
    if (Serial.read() == 's' && _screenshot_handler != nullptr)
    {
      _screenshot_handler();
      return;
    }
  }
}

bool GDTouchKeyboard::_isValidInput(const String& candidate, bool complete) const
{
  const uint16_t length = candidate.length();
  if ((complete && length < _minimum_length) ||
      (_maximum_length != 0 && length > _maximum_length))
  {
    return false;
  }
  return _input_validator == nullptr || _input_validator(candidate);
}

void GDTouchKeyboard::_startTouchFeedback()
{
  if (_touch_feedback)
  {
    M5.Power.setVibration(180);
    _vibration_stop_at = millis() + 80;
  }
}

String GDTouchKeyboard::_formatMacInput(const String& value) const
{
  String formatted;
  formatted.reserve(value.length() + (value.length() / 2));
  for (size_t index = 0; index < value.length(); ++index)
  {
    if (index != 0 && (index % 2) == 0)
    {
      formatted += ':';
    }
    formatted += value[index];
  }
  return formatted;
}

String GDTouchKeyboard::_stripMacSeparators(const String& value) const
{
  String stripped = value;
  stripped.replace(":", "");
  return stripped;
}

void GDTouchKeyboard::_updateInputText()
{
  M5.Display.setFont(font);
  M5.Display.setTextSize(1);
  M5.Display.setTextDatum(TL_DATUM);

  String visibleText = _key_mode == KEY_MODE_MAC
                          ? _formatMacInput(_input_text)
                          : _input_text;
  const bool showingPrompt = visibleText.length() == 0 && _prompt_text.length() > 0;
  if (showingPrompt)
  {
    visibleText = _prompt_text;
  }
  const int cursorWidth = 15;
  const int availableWidth = M5.Display.width() - cursorWidth - 2;
  while (visibleText.length() > 0 &&
         M5.Display.textWidth(visibleText) > availableWidth)
  {
    visibleText = visibleText.substring(1);
  }

  const int visibleWidth = M5.Display.textWidth(visibleText);
  M5.Display.fillRect(0, 0, M5.Display.width(), KEYBOARD_Y - 1,
                      _color_scheme.background);
  M5.Display.setTextColor(showingPrompt ? _color_scheme.promptText
                                        : _color_scheme.inputText,
                           _color_scheme.background);
  M5.Display.drawString(visibleText, 0, 10);
  M5.Display.setTextColor(_color_scheme.inputText, _color_scheme.background);
  if (_cursor_state == true && !showingPrompt)
  {
    M5.Display.fillRect(visibleWidth + 2, 2, cursorWidth,
                       KEYBOARD_Y - 6, _color_scheme.cursor);
  }
  _old_input_text = _input_text;
}

void GDTouchKeyboard::_initKeyboard(String text)
{
  M5.Display.fillScreen(_color_scheme.background);
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(_color_scheme.controlText, _color_scheme.background);
  M5.Display.setFont(font);
  M5.Display.setTextDatum(TC_DATUM);

  // Button A
  M5.Display.drawString("Delete", 55, 226);
  // Button B
  M5.Display.drawString("Done", 160, 226);
  // Button C
  M5.Display.drawString("Mode", 265, 226);

  _input_text = text;
  _old_input_text = "";
  _key_mode = KEY_MODE_LETTER;
  _shift_mode = false;
  _cursor_state = false;
  _cursor_last = millis();
}

void GDTouchKeyboard::_deinitKeyboard()
{
}

void GDTouchKeyboard::_drawKeyboard()
{
  const int visibleCols = (_key_mode == KEY_MODE_HEX ||
                           _key_mode == KEY_MODE_MAC) ? 4 : COLS;

  if (_key_mode == KEY_MODE_HEX || _key_mode == KEY_MODE_MAC)
  {
    M5.Display.fillRect(KEYBOARD_X + (visibleCols * KEY_W), KEYBOARD_Y,
                        (COLS - visibleCols) * KEY_W, ROWS * KEY_H,
                        _color_scheme.background);
  }

  for(int r = 0; r < ROWS; r++)
  {
    for(int c = 0; c < visibleCols; c++)
    {
      const int x = KEYBOARD_X + (c * KEY_W);
      const int y = KEYBOARD_Y + (r * KEY_H);

      int key_page = 0;

          if(_key_mode == KEY_MODE_NUMBER) key_page += 2;
          else if(_key_mode == KEY_MODE_HEX || _key_mode == KEY_MODE_MAC)
            key_page += 4;
           if(_shift_mode == true && _key_mode != KEY_MODE_HEX &&
             _key_mode != KEY_MODE_MAC) key_page += 1;

      String key;
      char ch = keymap[key_page][r][c];

      if(ch == '\002')  // Shift
      {
        key = "SHFT";
      }
      else
      {
        if(ch != '\001')
        {
          key = String(ch);
        }
      }
      const uint16_t fillColor = key.length() == 0 ? _color_scheme.background
                                                   : _color_scheme.keyBackground;
      M5.Display.fillRoundRect(x, y, KEY_W, KEY_H, 4, fillColor);
      M5.Display.drawRoundRect(x, y, KEY_W, KEY_H, 4, _color_scheme.keyBorder);
      M5.Display.setFont(font);
      M5.Display.setTextColor(_color_scheme.keyText, fillColor);
      M5.Display.setTextSize(key == "SHFT" ? 1 : 2);
      M5.Display.setTextDatum(MC_DATUM);
      M5.Display.drawString(key, x + (KEY_W / 2), y + (KEY_H / 2));
    }
  }
}

void GDTouchKeyboard::_processInput()
{
  if (M5.BtnA.wasReleasedAfterHold())
  {
    _input_text = isEditable ? "" : promptText;
    _updateInputText();
  }
  else if (M5.BtnA.wasClicked())
  {
    if (isEditable)
    {
      if (_input_text.length() > 0)
      {
        _input_text = _input_text.substring(0, _input_text.length() - 1);
      }
    }
    else if (_input_text.length() > promptText.length())
    {
      _input_text = _input_text.substring(0, _input_text.length() - 1);
    }
    _updateInputText();
  }
  else if (M5.BtnB.wasClicked())
  {
    if (_isValidInput(_input_text, true))
    {
      _keyboard_done = true;
    }
    return;
  }
  else if (M5.BtnC.wasClicked())
  {
    for (uint8_t offset = 1; offset <= 4; offset++)
    {
      const uint8_t mode = (static_cast<uint8_t>(_key_mode) + offset) % 4;
      if ((_available_modes & (1 << mode)) != 0)
      {
        _key_mode = static_cast<key_mode_t>(mode);
        break;
      }
    }
    _shift_mode = false;
    _drawKeyboard();
    return;
  }

  if (M5.Touch.getCount() == 0)
  {
    return;
  }

  const auto touch = M5.Touch.getDetail();
  if (!touch.wasPressed())
  {
    return;
  }

  const int visibleCols = (_key_mode == KEY_MODE_HEX ||
                           _key_mode == KEY_MODE_MAC) ? 4 : COLS;
  for (int r = 0; r < ROWS; r++)
  {
    for (int c = 0; c < visibleCols; c++)
    {
      const int x = KEYBOARD_X + (c * KEY_W);
      const int y = KEYBOARD_Y + (r * KEY_H);
      if (touch.x < x || touch.x >= x + KEY_W ||
          touch.y < y || touch.y >= y + KEY_H)
      {
        continue;
      }

      int key_page = 0;
      if (_key_mode == KEY_MODE_NUMBER) key_page += 2;
      else if (_key_mode == KEY_MODE_HEX || _key_mode == KEY_MODE_MAC)
        key_page += 4;
      if (_shift_mode && _key_mode != KEY_MODE_HEX && _key_mode != KEY_MODE_MAC) key_page += 1;

      const char ch = keymap[key_page][r][c];
      _startTouchFeedback();
      if (ch == '\002')
      {
        _shift_mode = !_shift_mode;
        _drawKeyboard();
      }
      else if (ch != '\001')
      {
        const String candidate = _input_text + String(ch);
        if (_isValidInput(candidate, false))
        {
          _input_text = candidate;
          _updateInputText();
        }
      }
      return;
    }
  }
}

GDTouchKeyboard GDTK;
