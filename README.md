# Gwendesign Touch Keyboard

Touch keyboard for M5Unified-compatible ESP32 devices.

## Forks

### @ejagombar/GDTouchKeyboard

Modified to allow for custom theming as well as several other improvements.

### @matthias-bs/GDTouchKeyboard

Additional changes:

- Ported the library and example to M5Unified and M5GFX.
- Added hexadecimal mode in addition to letter and number keyboard modes.
- Added MAC address mode with automatic colon insertion and optional separator preservation in the returned value.
- Added mode masks to restrict the available keyboard modes.
- Added minimum and maximum input-length limits.
- Added caller-supplied input validation callbacks.
- Added optional tactile feedback for touch input.
- Added optional prompt text for the keyboard input area.
- Added the `GDTouchKeyboardValidation` example, demonstrating validated configuration entries, mode restrictions, physical-button navigation, and tactile feedback.
- Added the `GDTouchKeyboardMac` example, demonstrating fixed-length MAC address entry with tactile feedback.

## Colour Scheme

By default, the keyboard uses the original GDTouchKeyboard colour scheme:
black keys, green key borders and physical-button labels, ocher key text and
cursor, and white input text. Configure every displayed colour before calling
`run()`:

```cpp
GDTouchKeyboard::color_scheme_t scheme = GDTK.getColorScheme();
scheme.background = TFT_NAVY;
scheme.keyBackground = TFT_DARKGREEN;
scheme.keyBorder = TFT_CYAN;
scheme.keyText = TFT_WHITE;
scheme.controlText = TFT_CYAN;
scheme.inputText = TFT_WHITE;
scheme.promptText = TFT_LIGHTGREY;
scheme.cursor = TFT_YELLOW;
GDTK.setColorScheme(scheme);
```

The `setColourIn` argument of `run()` remains supported for existing sketches.
When supplied, it overrides the configured key-border and cursor colours for
that and subsequent keyboard runs. Omit it, or pass
`GDTouchKeyboard::COLOR_SCHEME_UNCHANGED`, to retain the configured scheme.

## Optional Screenshot Capture

Screenshot support is provided by the optional
[M5SerialPngCapture](https://github.com/matthias-bs/M5SerialPngCapture)
Arduino library. Install it alongside GDTouchKeyboard, then enable the
integration in the `GDTouchKeyboardValidation` example:

```cpp
#define GDTOUCHKEYBOARD_ENABLE_SCREENSHOT 1
```

The example uses `M5SerialPngCapture` to send the current display over the
serial connection. GDTouchKeyboard itself does not depend on that library.

The M5SerialPngCapture repository includes the Linux utility
`tools/capture_screenshot.py`, which requests a screenshot and saves the
received PNG:

```text
python3 -m pip install pyserial
python3 /path/to/M5SerialPngCapture/tools/capture_screenshot.py \
  --port /dev/ttyACM0 --output screen.png
```

The protocol is `PNG1` followed by a 4-byte little-endian PNG length and the
PNG data. The example services requests while the configuration overview is
active, while GDTouchKeyboard services them during `GDTK.run()`.
