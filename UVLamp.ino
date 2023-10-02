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

#include <Arduino.h>
#include <EEPROM.h>
/*#include <TimerOne/TimerOne.h>*/
#include <TimerOne.h>
#include "Lcd16x2.h"
#include "UVLamp.h"

const int UV_PIN  = 6;

unsigned short currentCase = 0; // default case
unsigned short previousCase = 1; // default case
unsigned short prevprevCase = 0; // default case
unsigned short previousCaseTemp;
bool currentNodeChanged = false;
bool isFirsTime = true;

Menu *currentNode;

uint8_t selectedMemForSave;
uint8_t addrToStart = 0;
uint8_t startPrepareCase = 2;
int16_t countDownTime_var = 0;


void readProfileFromMem(){
////use it only for the eeprom initialization. this can be commented after is initialized to reduce space
// //-->
    uint8_t isDataInMem;
    EEPROM_read(0,isDataInMem);

    if(isDataInMem == 255){ // first time, eeprom is empty
        mem.selected = 0;
        mem.profiles[0].backlight = 1;
        mem.profiles[0].fan = 0;
        mem.profiles[0].uv = 100;
        mem.profiles[0].timer = 300;
        strncpy(mem.profiles[0].profile, "UV\0",6);

        mem.profiles[1].backlight = 1;
        mem.profiles[1].fan = 100;
        mem.profiles[1].uv = 10;
        mem.profiles[1].timer = 5;
        strncpy(mem.profiles[1].profile, "Green\0",6);

        EEPROM_write(addrToStart+1,mem);
        EEPROM_write(addrToStart,0); // isDataInMem = 1, meens that eeprom is already initialized
    } // <---
    EEPROM_read(addrToStart+1,mem);
    selectedMemForSave=mem.selected;
}

void initial() {
    readProfileFromMem();
    currentNode->parent = currentNode = &root; // same for root node, as it will be a loop
    saveCurrentCase(currentCase,previousCase,prevprevCase);
    currentNode->submenus = (Menu*)malloc(sizeof(Menu) * 7); // TODO change here with the number of menus

    currentNode = &currentNode->submenus[5];
    currentNode->submenus = (Menu*)malloc(sizeof(Menu) * 2); // TODO change here with the number of menus
    saveCurrentCase(0,1,1);

    currentNode = &root;
    if(isFirsTime) {
        createOrUpdateMenu();
        isFirsTime = false;
    }
}

void createOrUpdateMenu() {
    char textValue[6];
    addMenu(0, "Profile"  , "%-9s %5s", 5, mem.profiles[mem.selected].profile);
    addMenu(1,"Backlight","%-9s %5s",  1,onOFF(mem.profiles[mem.selected].backlight,textValue));
    addMenu(2,"Fan"      ,"%-9s %5s",  2,autoOff(mem.profiles[mem.selected].fan, textValue));
    addMenu(3,"Timer"    ,"%-9s %s",   3,timer(mem.profiles[mem.selected].timer, textValue));
    addMenu(4, "UV"       , "%-9s %4s%%", 4, String(mem.profiles[mem.selected].uv).c_str());
    addMenu(5,"Save"     ,"%-9s %5s",  0,"");
    addMenu(6,"Start"    ,"%-9s %5s",  10,"");

    // adding submenu to Save
    currentNode = &currentNode->submenus[5];
    addMenu(0,"Profile", "%-9s %5s", 6, mem.profiles[mem.selected].profile);
    addMenu(1,"Save"   , "%-9s %5s", 7, "");

    //changing back the currentNode to root
    currentNode = &root;
}

void addMenu(const signed short index, const char * menuText,const char *formater, uint8_t menuType,const char *value){
    currentNode->submenus[index].parent = currentNode;
    currentNode->submenus[index].text.menuText = menuText;
    currentNode->submenus[index].text.formater = formater;
    currentNode->submenus[index].menuType = menuType;
    strncpy(currentNode->submenus[index].text.value,value,6);
    currentNode->submenus[index].text.value[5]= '\0';
    if(isFirsTime) {
        currentNode->submenus[index].noOfSubMenus = 0;
        currentNode->noOfSubMenus = currentNode->noOfSubMenus + 1;
    }
}

void setup() {
//    noInterrupts();
//    interrupts();
    Timer1.initialize(1000000);  // 1000000 microseconds = 1 second
    pinMode(UV_PIN, OUTPUT);


//    analogReference(INTERNAL);
   //Serial.begin(9600);
    initial();
    Lcd16x2Init();
    setLcdBacklight(mem.profiles[mem.selected].backlight);

    getLcd().setCursor(0,0);
    getLcd().write(1);
    casesScenario(btnNONE);
}

void loop() {
    lcdKey = read_LCD_buttons();  // read the buttons
    if (allowLongerPress || (buttonJustReleased || buttonJustPressed)) {
        previousCaseTemp = currentCase;
        switch (lcdKey){               // depending on which button was pushed, we perform an action
            case btnUP: {
                if (!selectedMode){
                    currentCase = currentCase == 0 ? currentNode->noOfSubMenus - 1 : currentCase  - 1 ;
                }
                break;
            }
            case btnDOWN: {
                if (!selectedMode) {
                    currentCase = currentCase == (currentNode->noOfSubMenus - 1) ? 0 : currentCase +1;
                }
                break;
            }
            case btnRESET: {
                if(selectedMode)
                    selectedMode = false;
                else if(currentNode->noOfSubMenus != 0){
                    saveCurrentCase(currentCase,previousCase,prevprevCase);
                    loadNewNodeCurrentCase(currentNode->parent);
                    currentNodeChanged = true;
                }
                startPrepareCase = 2;
                analogWrite(UV_PIN, 0);
                Timer1.stop();
                break;
            }
            case btnSELECT: {
                if(currentNode->submenus[currentCase].noOfSubMenus == 0 && currentNode->submenus[currentCase].menuType != 0)
                    selectedMode = true;
                else if(currentNode->submenus[currentCase].noOfSubMenus != 0){
                    selectedMode = false;
                    currentNodeChanged = true;
                    saveCurrentCase(currentCase,previousCase,prevprevCase);
                    loadNewNodeCurrentCase(&(currentNode->submenus[currentCase]));
                }
                break;
            }
        }
        if(previousCaseTemp != currentCase && !currentNodeChanged){
            prevprevCase = previousCase;
            previousCase = previousCaseTemp;
        }
        casesScenario(lcdKey);
        currentNodeChanged = false;
    }
    if (buttonJustPressed){
        buttonJustPressed = false;
    }
    if (buttonJustReleased) {
        allowLongerPress = false;
        buttonJustReleased = false;
    }
    //save the latest button value, for change event detection next time round
    buttonWas = lcdKey;
}

void casesScenario(const unsigned short &lcd_key) {

    if(!selectedMode && (lcd_key!=btnRESET || lcd_key!=btnSELECT)) {

        if (prevprevCase == currentCase && (lcd_key == btnUP || lcd_key == btnDOWN)) { // case when we move just arrow
            setArrow(lcd_key == btnUP);
        } else{ // case when we move the previous and introduce another current menu
            if(currentNodeChanged){
                setArrow(currentCaseIsTop());
            }
            createLineMenu(0,currentCaseIsTop());
            createLineMenu(1,!currentCaseIsTop());
        }

    }else { // selectedMode case;// selectedMode = true
        switch (currentNode->submenus[currentCase].menuType) {
            case 1: //Backlight
                selectedFunctionLogic(lcd_key, 0, 1, 1, (int8_t *) &(mem.profiles[mem.selected].backlight));
                onOFF(mem.profiles[mem.selected].backlight, currentNode->submenus[currentCase].text.value);
                setLcdBacklight(mem.profiles[mem.selected].backlight);
                printSelectedCaseValue(currentNode->submenus[currentCase].text.value);
                break;
            case 2: // FAN, 255 = AUTO
                allowLongerPress = true;
                selectedFunctionLogic(lcd_key, -1, 100, 10, &(mem.profiles[mem.selected].fan));
                autoOff( mem.profiles[mem.selected].fan, currentNode->submenus[currentCase].text.value);
                printSelectedCaseValue(currentNode->submenus[currentCase].text.value);
                break;
            case 3: //TIMER
                allowLongerPress = true;
                selectedFunctionLogic(lcd_key, 5, 3600, 30, (int16_t *) &mem.profiles[mem.selected].timer);
                timer(mem.profiles[mem.selected].timer, currentNode->submenus[currentCase].text.value);
                printSelectedCaseValue(currentNode->submenus[currentCase].text.value);
                break;
            case 4: //UV
                allowLongerPress = true;
                selectedFunctionLogic(lcd_key, 10, 100, 10, &mem.profiles[mem.selected].uv);
                sprintf(currentNode->submenus[currentCase].text.value, "%4d%%", mem.profiles[mem.selected].uv);
                printSelectedCaseValue(currentNode->submenus[currentCase].text.value);
                break;
            case 5: // Profile
                selectedFunctionLogic(lcd_key, 0, 1, 1, (int8_t *) &mem.selected);
                createOrUpdateMenu();
                snprintf(currentNode->submenus[currentCase].text.value, 6, "%5s", mem.profiles[mem.selected].profile);
                currentNode->submenus[currentCase].text.value[5]= '\0';
                setLcdBacklight(mem.profiles[mem.selected].backlight);
                printSelectedCaseValue(currentNode->submenus[currentCase].text.value);
                break;
            case 6: // Save -> Profile
                selectedFunctionLogic(lcd_key, 0, 1, 1, (int8_t *) &selectedMemForSave);
                snprintf(currentNode->submenus[currentCase].text.value, 6, "%5s", mem.profiles[selectedMemForSave].profile);
                printSelectedCaseValue(currentNode->submenus[currentCase].text.value);
                break;
            case 7: // Save -> Save
                // copy from localCache & save to desired Profile
                mem.profiles[selectedMemForSave].backlight = mem.profiles[mem.selected].backlight;
                mem.profiles[selectedMemForSave].fan = mem.profiles[mem.selected].fan;
                mem.profiles[selectedMemForSave].uv = mem.profiles[mem.selected].uv;
                mem.profiles[selectedMemForSave].timer = mem.profiles[mem.selected].timer;
                //1byte - flag for eeprom init,1byte for mem.selected and 11bytes has Profile
                EEPROM_write(addrToStart+2+(11*selectedMemForSave) , mem.profiles[selectedMemForSave]);
                EEPROM_write(addrToStart+1,selectedMemForSave); // changing selectedValue
                readProfileFromMem();
                loadNewNodeCurrentCase(currentNode->parent);
                currentNodeChanged = true;
                selectedMode = false;
                casesScenario(btnNONE);
                break;
            case 10: // Start
                if(buttonJustReleased && buttonWas == btnSELECT){
                    getLcd().clear();
                    setArrow(currentCaseIsTop());
                    createLineMenu(!currentCaseIsTop(),1);
//                        getLcd().setCursor(1, currentCaseIsTop());
//                        getLcd().print(buttonWas);
                    startPrepareCase--;
                    //start red leds
                    if(startPrepareCase == 0){
                        //startPrepareCase = 2;
                       // delay(1000 * 5); // 5 sec to for eye safe
                        countDownTime_var = mem.profiles[mem.selected].timer;

                        Timer1.attachInterrupt(countDownTime);
                        Timer1.stop();
                        Timer1.start();
                        //startTimer interrupt
                    }
                }
                break;
        }
    }
}
void countDownTime(){
    char value[6];
//    getLcd().setCursor(1, currentCaseIsTop());
//    getLcd().print(countDownTime_var);
    if(countDownTime_var >= 0){
        analogWrite(UV_PIN, ((float)255/100)*mem.profiles[mem.selected].uv);
        getLcd().setCursor(1, currentCaseIsTop());
        getLcd().print(timer(countDownTime_var,value));
        countDownTime_var--;
    }else{
        analogWrite(UV_PIN, 0);
        Timer1.stop();
        startPrepareCase = 2 ;
        //start red leds
    }
}

void createLineMenu(uint8_t isSecondLine,uint8_t isTopCurrentCase) {
    char line[16];
    getLcd().setCursor(1, isSecondLine);
    snprintf(line,16,
                    currentNode->submenus[isTopCurrentCase ? currentCase : previousCase].text.formater,
                    currentNode->submenus[isTopCurrentCase ? currentCase : previousCase].text.menuText,
                    currentNode->submenus[isTopCurrentCase ? currentCase : previousCase].text.value
            );
    line[15]='\0';
    getLcd().print(line);
    delay(50);
}
void setArrow(bool isTop) {
    getLcd().setCursor(0, isTop);
    getLcd().print(" ");
    getLcd().setCursor(0, !isTop);
    getLcd().write(1);
}
char * autoOff(int8_t intValue, char * string){
    int value = 0xFF & (int) intValue;
    if ( value == 255){
        snprintf(string, 6, "%5s", "AUTO");
    }else if( value == 0) {
        snprintf(string,6, "%5s", "OFF");
    }else {
        snprintf(string, 6,"%4d%%", value);
    }
    string[5]='\0';
    return string;
}
char * timer(int intValue, char * string){
    snprintf( string, 6, "%02d:%02d",  intValue / 60, intValue % 60 );
    string[5]='\0';
    return string;
}
char * onOFF(int intValue, char * string){
    if(intValue == 1){
        snprintf( string,6, "%5s", "ON");
    }else{
        snprintf( string,6, "%5s", "OFF");
    }
    string[5]='\0';
    return string;
}
void selectedFunctionLogic(const unsigned short &lcd_key,const signed short min, const signed short max,const unsigned short iStep, int8_t * from) {
    if (lcd_key < btnSELECT) {
        int8_t iStepT =
                lcd_key == btnLEFT ? -1 : lcd_key == btnRIGHT ? 1 : lcd_key == btnUP ? iStep : lcd_key == btnDOWN ? -iStep : 0;
        *from = iStepT>= 0 ? ( *from == max ? min : *from < (max + iStepT) ? (*from + iStepT) : max) : (*from == min ? max : min > (*from + iStepT) ? max : *from + iStepT);
        delay(200);
    }
}
void selectedFunctionLogic(const unsigned short &lcd_key,const signed short min, const signed short max,const unsigned short iStep, int16_t * from){
    if(lcd_key < btnSELECT ){
        int8_t iStepT = lcd_key == btnLEFT ? -1 : lcd_key == btnRIGHT ? 1 : lcd_key == btnUP ? iStep : lcd_key == btnDOWN ? -iStep : 0;
        *from = iStepT>= 0 ? ( *from == max ? min : *from < (max + iStepT) ? (*from + iStepT) : max) : (*from == min ? max : min > (*from + iStepT) ? max : *from + iStepT);
        delay(200);
    }
}
uint8_t currentCaseIsTop(){
    return !((currentCase > previousCase && !(currentCase == currentNode->noOfSubMenus - 1 && previousCase == 0)) ||
           (currentCase == 0 && previousCase == currentNode->noOfSubMenus - 1));
}
void saveCurrentCase(const unsigned short &_currentCase, const unsigned short &_previousCase,const unsigned short &_prevprevCase) {
    currentNode->currentCase=_currentCase;
    currentNode->previousCase=_previousCase;
    currentNode->prevprevCase=_prevprevCase;
}
void loadNewNodeCurrentCase(struct Menu * newCurrentNode) {
    currentNode = newCurrentNode;
    currentCase = currentNode->currentCase;
    previousCase = currentNode->previousCase;
    prevprevCase = currentNode->prevprevCase;
}
void printSelectedCaseValue(char * value) {
    getLcd().setCursor(10, !currentCaseIsTop());
    getLcd().write(1);
    getLcd().write(value);
}
