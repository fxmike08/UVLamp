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

#ifndef UVLAMP_UVLAMP_H
#define UVLAMP_UVLAMP_H



typedef struct Text{

    const char *formater; //default format
    const char *menuText;
    char value[6];
} Text;


typedef struct Menu {
    Text text;
    unsigned short noOfSubMenus;
    uint8_t menuType;
    struct Menu *parent;
    struct Menu *submenus;
    unsigned short currentCase;
    unsigned short previousCase;
    unsigned short prevprevCase;
} Menu;
Menu root;


typedef struct Profile{ // totally 11 bytes
    uint8_t backlight; //1 bytes
    int8_t fan; //1 bytes
    int8_t uv; //1 bytes
    uint16_t timer; //2 bytes
    char profile[6]; // 6 bytes
} Profile;
typedef struct EepromMem{
    uint8_t selected; //1 byte
    struct Profile profiles [2];
}EepromMem;
EepromMem mem;


template <class T> int EEPROM_write(int startingAddr, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
        EEPROM.write(startingAddr++, *p++);
    return i;
}

template <class T> int EEPROM_read(int startingAddr, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
        *p++ = EEPROM.read(startingAddr++);
    return i;
}


#endif //UVLAMP_UVLAMP_H
void printSelectedCaseValue(char * value);
void readProfileFromMem();
uint8_t currentCaseIsTop();

void createOrUpdateMenu();
void createLineMenu(uint8_t isSecondLine,uint8_t isTopCurrentCase);
void setArrow(bool isTop);
char * timer(int intValue, char * string);
char * onOFF(int intValue, char * string);
char * autoOff(int8_t intValue, char * string);
void casesScenario(const unsigned short &lcd_key);
void loadNewNodeCurrentCase(struct Menu * newCurrentNode);
void addMenu(const signed short index, const char * menuText, const char *formater, uint8_t menuType, const char *value);
void saveCurrentCase(const unsigned short &currentCase, const unsigned short &previousCase,const unsigned short &prevprevCase);
void selectedFunctionLogic(const unsigned short &lcd_key,const signed short min, const signed short max,const unsigned short iStep, int16_t * from);
void selectedFunctionLogic(const unsigned short &lcd_key,const signed short min, const signed short max,const unsigned short iStep, int8_t * from);




