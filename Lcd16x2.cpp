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

// Created by Mike on 1/3/2017.
//
#include <LiquidCrystal.h>
#include "Lcd16x2.h"

#ifndef UVLAMP_LCD16X2
#define UVLAMP_LCD16X2
//Pins for the 16x2 LCD shield. LCD: ( RS, E, LCD_D4, LCD_D5, LCD_D6, LCD_D7 )
LiquidCrystal lcd(RS, E, LCD_D4, LCD_D5, LCD_D6, LCD_D7 );
#endif //UVLAMP_LCD16X2

void Lcd16x2Init(){
    pinMode( btnDigReset, INPUT );
    lcd.createChar(1, arrowR); // defining arrowRight for selection
    lcd.begin( 16, 2 );
    lcd.clear();
}


unsigned short read_LCD_buttons() {
    pressedBtn = btnNONE;
    resetBtn = digitalRead(btnDigReset);
    lcdKey = 0 + analogRead(BUTTON_ADC_PIN);      // read the value from the sensor

        /* uncomment following for debugging */
//    lcd.setCursor( 10, 1 );   //bottom left
//    lcd.print( "                  " );
//    lcd.setCursor( 10, 1 );   //bottom left
//    lcd.print( lcdKey );
//    delay(500);

    if (lcdKey < 50)        pressedBtn = btnRIGHT;
    else if (lcdKey < 200)  pressedBtn = btnUP;
    else if (lcdKey < 380)  pressedBtn = btnDOWN;
    else if (lcdKey < 600)  pressedBtn = btnLEFT;
    else if (lcdKey < 900)  pressedBtn = btnSELECT;
    else if (resetBtn == LOW)   pressedBtn = btnRESET;

    //handle button flags for just pressed and just released events
    if ( ( buttonWas == btnNONE ) && ( pressedBtn != btnNONE ) )
    {
        //the button was just pressed, set buttonJustPressed, this can optionally be used to trigger a once-off action for a button press event
        //it's the duty of the receiver to clear these flags if it wants to detect a new button change event
        buttonJustPressed  = true;
        buttonJustReleased = false;
    }
    if ( ( buttonWas != btnNONE ) && ( pressedBtn == btnNONE ) )
    {
        buttonJustPressed  = false;
        buttonJustReleased = true;
    }
    return pressedBtn;
}

LiquidCrystal getLcd(){
    return lcd;
}

void setLcdBacklight(uint8_t state){
    if( state ){
        pinMode( LCD_D3_BACKLIGHT_PIN, INPUT );
    }else {
        pinMode(LCD_D3_BACKLIGHT_PIN, OUTPUT);
        digitalWrite(LCD_D3_BACKLIGHT_PIN, LOW);
    }
}
