//# Copyright (C) 2023 Dulgheru Mihaita (fxmike08)
//# This file is part of UVLamp <https://github.com/fxmike08/UVLamp>.

//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as
//published by the Free Software Foundation, either version 3 of the
//License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public
//License along with this program.  If not, see
//<http://www.gnu.org/licenses/gpl-3.0.html>.

//
// Created by Mike on 1/3/2017.
//
#include <stdint.h>

#ifndef UVLAMP_LCD16X2_H
#define UVLAMP_LCD16X2_H
//--->
uint8_t arrowR[8] = { 0x0, 0x4, 0x2, 0x1F, 0x02, 0x04, 0x0, 0x0};
/*------LCD_PIN----------------mC_PIN------------*/
#define RS                          10 //    // old mapping /8
#define E                           11 //    // old mapping /13
#define LCD_D4                      7 //   // old mapping /12
#define LCD_D5                      13 //   // old mapping /11
#define LCD_D6                      8 //   // old mapping /10
#define LCD_D7                      9 //    // old mapping /9
#define LCD_D3_BACKLIGHT_PIN         12 // D3 controls LCD backlight  // // old mapping /7

/*------Name-------------------Values------------*/
#define btnRIGHT                0       //value returned by read_LCD_buttons()
#define btnUP                   1       //value returned by read_LCD_buttons()
#define btnDOWN                 2       //value returned by read_LCD_buttons()
#define btnLEFT                 3       //value returned by read_LCD_buttons()
#define btnSELECT               4       //value returned by read_LCD_buttons()
#define btnNONE                 5       //value returned by read_LCD_buttons()
#define btnRESET                6       //value returned by read_LCD_buttons()
#define btnDigReset             2       // Digital pin 7 from ardunino, don't forget to put a resistor 4k for pullup
#define BUTTON_ADC_PIN          0        // A0 is the button ADC input
uint8_t buttonJustPressed, buttonJustReleased  = false;

uint8_t selectedMode = false; // used when we select a case
uint8_t allowLongerPress = false;
unsigned short buttonWas = btnNONE;                  // default btnNONE = 5
unsigned short pressedBtn,resetBtn;
unsigned short lcdKey;
//<---
#endif //UVLAMP_LCD16X2_H

/*-----declaring functions------*/
unsigned short read_LCD_buttons();
void Lcd16x2Init();
LiquidCrystal getLcd();
void setLcdBacklight(uint8_t state);
