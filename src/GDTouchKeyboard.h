// Copyright (c) GWENDESIGN. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef _GDTOUCHKEYBOARD_H_
#define _GDTOUCHKEYBOARD_H_

#include <M5Unified.h>




class GDTouchKeyboard
{
public:
  /** Keyboard layouts supported by the library. */
  typedef enum
  {
    KEY_MODE_LETTER = 0,
    KEY_MODE_NUMBER = 1,
    KEY_MODE_HEX = 2,
    KEY_MODE_MAC = 3,
  } key_mode_t;

  /** Callback used to accept or reject a candidate input string. */
  typedef bool (*input_validator_t)(const String& candidate);

  /**
   * @brief Colours used to render the keyboard.
   *
   * The default values match the original GDTouchKeyboard palette.
   */
  struct color_scheme_t
  {
    uint16_t background = TFT_BLACK;
    uint16_t inputText = TFT_GREEN;
    uint16_t promptText = TFT_DARKGREY;
    uint16_t cursor = 0xC460;
    uint16_t keyBackground = TFT_BLACK;
    uint16_t keyBorder = 0xC460;
    uint16_t keyText = TFT_GREEN;
    uint16_t controlText = TFT_GREEN;
  };

  static const uint16_t COLOR_SCHEME_UNCHANGED = 0xFFFF;

  /** Create a touch keyboard instance. */
  GDTouchKeyboard();

  /** Release resources owned by the touch keyboard instance. */
  ~GDTouchKeyboard();

  /**
   * @brief Show the keyboard and wait until the user accepts the input.
   *
   * The prompt is displayed as placeholder text while the input is empty. It
   * is not included in the returned value and is replaced by user input.
   *
   * @param text Initial input value.
   * @param setColourIn Optional legacy 16-bit RGB565 accent colour used for
   * keyboard borders and the cursor. Use COLOR_SCHEME_UNCHANGED (the default)
   * to retain the configured colour scheme.
   * @param getIsEditable If true, the initial value can be edited and deleted.
   * @param fontIn Font used to render the keyboard and input.
   * @param modeIn Initial keyboard mode. Falls back to an available mode when
   * the requested mode is disabled.
   * @param preserveMacSeparators Return MAC addresses with colon separators.
   * @param prompt Optional placeholder text shown above the keyboard.
   * @return The value accepted by the user.
   */
  String run(String text = "", uint16_t setColourIn = COLOR_SCHEME_UNCHANGED,
             bool getIsEditable = true,
             const lgfx::v1::IFont* fontIn = &fonts::Font0,
             key_mode_t modeIn = KEY_MODE_LETTER,
             bool preserveMacSeparators = true,
             String prompt = "");

  /** Set the colours used to render the keyboard. */
  void setColorScheme(const color_scheme_t& scheme);

  /** Return the colours currently used to render the keyboard. */
  const color_scheme_t& getColorScheme(void) const;

  /** Set the active keyboard mode. */
  void setMode(key_mode_t modeIn);

  /**
   * @brief Restrict the modes available through the Mode button.
   *
   * Each bit in modeMask corresponds to the numeric value of a key_mode_t.
   * A zero mask restores all supported modes.
   */
  void setAvailableModes(uint8_t modeMask);

  /**
   * @brief Set input length limits.
   *
   * A maximum length of zero disables the upper limit. The minimum length is
   * checked when the user presses Done.
   */
  void setInputLength(uint16_t minLength, uint16_t maxLength = 0);

  /** Set a callback used to validate input as it is entered and on completion. */
  void setInputValidator(input_validator_t validator);

  /** Enable or disable vibration feedback for touch input. */
  void setTouchFeedback(bool enabled);

private:

  #define KEYBOARD_X (2)
  #define KEYBOARD_Y (26)

  #define KEY_W (45)
  #define KEY_H (50)

  #define COLS (7)
  #define ROWS (4)

  #define MAX_SHIFT_MODE (6)

  const char keymap[MAX_SHIFT_MODE][ROWS][COLS] =
  {
    {
    {'a', 'b', 'c', 'd', 'e', 'f', 'g'},
    {'h', 'i', 'j', 'k', 'l', 'm', 'n'},
    {'o', 'p', 'q', 'r', 's', 't', 'u'},
    {'v', 'w', 'x', 'y', 'z', ' ', '\002'}, // 002 = shift
    },
    {
    {'A', 'B', 'C', 'D', 'E', 'F', 'G'},
    {'H', 'I', 'J', 'K', 'L', 'M', 'N'},
    {'O', 'P', 'Q', 'R', 'S', 'T', 'U'},
    {'V', 'W', 'X', 'Y', 'Z', ' ', '\002'}, // 002 = shift
    },
    {
    {'0', '1', '2', '3', '4', '5', '6'},
    {'7', '8', '9', '`', '-', '=', '['},
    {']', '\\', ';', '\'', ',', '.', '/'},
    {' ', ' ', ' ', ' ', ' ', ' ', '\002'}, // 002 = shift
    },
    {
    {'~', '!', '@', '#', '$', '%', '^'},
    {'&', '*', '(', ')', '_', '+', '{'},
    {'}', '|', ':', '"', '<', '>', '?'},
    {' ', ' ', ' ', ' ', ' ', ' ', '\002'}, // 002 = shift
    },
    {
    {'0', '1', '2', '3', '\001', '\001', '\001'},
    {'4', '5', '6', '7', '\001', '\001', '\001'},
    {'8', '9', 'A', 'B', '\001', '\001', '\001'},
    {'C', 'D', 'E', 'F', '\001', '\001', '\001'},
    },
    {
    {'0', '1', '2', '3', '\001', '\001', '\001'},
    {'4', '5', '6', '7', '\001', '\001', '\001'},
    {'8', '9', 'A', 'B', '\001', '\001', '\001'},
    {'C', 'D', 'E', 'F', '\001', '\001', '\001'},
    },
  };



  void _processInput(void);
  void _updateInputText(void);
  void _initKeyboard(String text = "");
  void _deinitKeyboard(void);
  void _drawKeyboard(void);
  bool _isValidInput(const String& candidate, bool complete) const;
  void _startTouchFeedback(void);
  String _formatMacInput(const String& value) const;
  String _stripMacSeparators(const String& value) const;



  color_scheme_t _color_scheme;
  String _input_text = "";
  String _old_input_text = "";
  key_mode_t _key_mode = KEY_MODE_LETTER;
  bool _shift_mode = false;
  bool _keyboard_done = false;
  uint32_t _cursor_last;
  bool _cursor_state = false;
  const lgfx::v1::IFont* font = &fonts::Font0;
  bool isEditable = false;
  String promptText = "";
  String _prompt_text = "";
  uint8_t _available_modes = 0x07;
  uint16_t _minimum_length = 0;
  uint16_t _maximum_length = 0;
  input_validator_t _input_validator = nullptr;
  bool _touch_feedback = false;
  uint32_t _vibration_stop_at = 0;
  bool _preserve_mac_separators = true;
};

extern GDTouchKeyboard GDTK;

#endif // _GDTOUCHKEYBOARD_H_
