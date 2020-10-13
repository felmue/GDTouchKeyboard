// Copyright (c) GWENDESIGN. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "GDTouchKeyboard.h"
#include <Free_Fonts.h>

#define KEYBOARD_X (2)
#define KEYBOARD_Y (26)

#define KEY_W (45)
#define KEY_H (50)

#define COLS (7)
#define ROWS (4)

#define MAX_SHIFT_MODE (4)

#define COLOR_OCHER (0xC460)

typedef enum
{
  KEY_MODE_LETTER = 0,
  KEY_MODE_NUMBER = 1,
} key_mode_t;

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
	{'`', '1', '2', '3', '4', '5', '6'},
	{'7', '8', '9', '0', '-', '=', '['},
	{']', '\\', ';', '\'', ',', '.', '/'},
	{' ', ' ', ' ', ' ', ' ', ' ', '\002'}, // 002 = shift
  },
  {
	{'~', '!', '@', '#', '$', '%', '^'},
	{'&', '*', '(', ')', '_', '+', '{'},
	{'}', '|', ':', '"', '<', '>', '?'},
	{' ', ' ', ' ', ' ', ' ', ' ', '\002'}, // 002 = shift
  },
};

Button *_button_list[ROWS][COLS];
String _input_text = "";
String _old_input_text = "";
key_mode_t _key_mode = KEY_MODE_LETTER;
bool _shift_mode = false;
bool _keyboard_done = false;
uint32_t _cursor_last;
bool _cursor_state = false;
ButtonColors _bc_on = {BLUE, GREEN, COLOR_OCHER};
ButtonColors _bc_off = {BLACK, GREEN, COLOR_OCHER};

static void _updateInputText(void);
static void _initKeyboard(String text = "");
static void _deinitKeyboard(void);
static void _btnAEvent(Event& e);
static void _buttonEvent(Event& e);
static void _drawKeyboard(void);

GDTouchKeyboard::GDTouchKeyboard()
{
}

GDTouchKeyboard::~GDTouchKeyboard()
{
}

String GDTouchKeyboard::run(String text)
{
  _initKeyboard(text);
  _drawKeyboard();
  _keyboard_done = false;
  while(_keyboard_done == false)
  {
    M5.update();

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
  _deinitKeyboard();
  return _input_text;
}

static void _updateInputText()
{
  int oitw = M5.Lcd.textWidth(_old_input_text);
  int itw = M5.Lcd.textWidth(_input_text);

  M5.Lcd.setFreeFont(FF2);
  if(_old_input_text != _input_text)
  {
    _old_input_text = _input_text;
    M5.Lcd.fillRect(0, 0, max(oitw, itw) + 40, KEYBOARD_Y - 1, TFT_BLACK);
    M5.Lcd.drawString(_input_text, 0, 10);
  }
  else
  {
    if(_cursor_state == true)
    {
      M5.Lcd.fillRect(itw + 2, 2, 15, KEYBOARD_Y - 6, COLOR_OCHER);
    }
    else
    {
      M5.Lcd.fillRect(itw + 2, 2, 15, KEYBOARD_Y - 6, TFT_BLACK);
    }
  }
}

static void _initKeyboard(String text)
{
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(TFT_GREEN, TFT_BLACK);
  M5.Lcd.setFreeFont(FF1);
  M5.Lcd.setTextDatum(TC_DATUM);

  // Button A
  M5.Lcd.drawString("delete", 55, 226, 2);
  // Button B
  M5.Lcd.drawString("done", 160, 226, 2);
  // Button C
  M5.Lcd.drawString("mode", 265, 226, 2);

  for(int r = 0; r < ROWS; r++)
  {
    for(int c = 0; c < COLS; c++)
    {
      _button_list[r][c] = new Button(0, 0, 0, 0, false, "", _bc_off, _bc_on);
      _button_list[r][c]->setTextSize(1);
    }
  }

  M5.Buttons.addHandler(_buttonEvent, E_TOUCH);
  M5.Buttons.addHandler(_btnAEvent, E_RELEASE);

  _input_text = text;
  _key_mode = KEY_MODE_LETTER;
  _shift_mode = false;
}

static void _deinitKeyboard()
{
  M5.Buttons.delHandlers(_buttonEvent, nullptr, nullptr);
  M5.Buttons.delHandlers(_btnAEvent, nullptr, nullptr);

  for(int r = 0; r < ROWS; r++)
  {
    for(int c = 0; c < COLS; c++)
    {
      delete(_button_list[r][c]);
      _button_list[r][c] = NULL;
    }
  }
}

static void _drawKeyboard()
{
  int x, y;

  for(int r = 0; r < ROWS; r++)
  {
    for(int c = 0; c < COLS; c++)
    {
      x = (KEYBOARD_X + (c * KEY_W));
      y = (KEYBOARD_Y + (r * KEY_H));
      _button_list[r][c]->set(x, y, KEY_W, KEY_H);

      int key_page = 0;

      if(_key_mode == KEY_MODE_NUMBER) key_page += 2;
      if(_shift_mode == true) key_page += 1;

      String key;
      char ch = keymap[key_page][r][c];

      if(ch == '\002')  // Shift
      {
        _button_list[r][c]->setFreeFont(FF1);
        key = "shft";
      }
      else
      {
        _button_list[r][c]->setFreeFont(FF3);
        key = String(ch);
      }
      _button_list[r][c]->setLabel(key.c_str());
      _button_list[r][c]->draw();
    }
  }
}

static void _btnAEvent(Event& e)
{
  // Delete all (long press) or delete one char (short press)
  if(e.button == &M5.BtnA)
  {
    if(e.duration > 500)
    {
      _input_text = "";
    }
    else
    {
      _input_text = _input_text.substring(0, _input_text.length() - 1);
    }
    _updateInputText();
  }
}

static void _buttonEvent(Event& e)
{
  Button& b = *e.button;

  // Delete
  if(e.button == &M5.BtnA)
  {
    // Ignore - handled in btnAEvent()
    return;
  }
  // Done
  else if(e.button == &M5.BtnB)
  {
    _keyboard_done = true;
    return;
  }
  // Key mode
  else if(e.button == &M5.BtnC)
  {
    switch(_key_mode)
    {
      case KEY_MODE_LETTER:
        _key_mode = KEY_MODE_NUMBER;
        break;
      default:
      case KEY_MODE_NUMBER:
        _key_mode = KEY_MODE_LETTER;
        break;
    }
    _shift_mode = false;
    _drawKeyboard();
    return;
  }
  else if(e.button == &M5.background)
  {
    // Ignore default background button
    return;
  }
  else
  {
    if(String(b.label()) == "shft")
    {
      _shift_mode = !_shift_mode;
      _drawKeyboard();
      return;
    }
    _input_text += b.label();
  }
  _updateInputText();
}

GDTouchKeyboard GDTK;
