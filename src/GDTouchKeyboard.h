// Copyright (c) GWENDESIGN. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef _GDTOUCHKEYBOARD_H_
#define _GDTOUCHKEYBOARD_H_

#include <M5Core2.h>

class GDTouchKeyboard
{
public:
  GDTouchKeyboard();
  ~GDTouchKeyboard();

  String run(String text = "");
};

extern GDTouchKeyboard GDTK;

#endif // _GDTOUCHKEYBOARD_H_
