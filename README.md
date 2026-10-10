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

## Screenshot Capture

The Linux utility [`tools/capture_screenshot.py`](tools/capture_screenshot.py)
requests a screenshot over the serial connection and saves the received PNG:

```text
python3 -m pip install pyserial
python3 tools/capture_screenshot.py --port /dev/ttyACM0 --output screen.png
```

The sketch must handle the default `s` request and respond with a binary frame:

```text
PNG1 + 4-byte little-endian PNG length + PNG data
```

Human-readable serial output before `PNG1` is ignored. The utility validates
the PNG signature, enforces an 8 MiB default size limit, and writes the image
through a temporary file before replacing the output file.

The `GDTouchKeyboardValidation` example includes the screenshot handler behind
the compile-time option `GDTOUCHKEYBOARD_ENABLE_SCREENSHOT`, which defaults to
`0`. Set it to `1` near the top of the sketch to capture both the configuration
overview and keyboard screens. The keyboard library services the request while
`GDTK.run()` is active; the example services it while the overview is active.
