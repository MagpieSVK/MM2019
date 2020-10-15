/** Display module */

#include "gl_hd.h"

LiquidCrystal lcd(19, 18, 17, 16, 15, 14);

static uint8_t DisplayMode;
static uint8_t NextDisplayMode;
static uint8_t DisplayTime;
static uint8_t NextDisplayTime;

void InicDisplay(void) {

  lcd.begin(16, 2);
  Serial.begin(9600);
  lcd.clear();
  DisplayMode = bootScreen;
  NextDisplayMode = voltageDC;
  DisplayTime = 2;
  NextDisplayTime = 0;

}

void SetDisplay(void) {
  
  static uint32_t ReferenceTime = millis();
  static bool Refresh = true;
  uint32_t TempTime1; 
  uint8_t HelpVar1 = 0;
  uint8_t HelpVar2 = 0;

  TempTime1 = millis();
  HelpVar1 = (uint8_t)((TempTime1 - ReferenceTime)/ms_in_s);
  if((DisplayMode != NextDisplayMode) && (DisplayTime <= HelpVar1)) {
    DisplayMode = NextDisplayMode;
    DisplayTime = NextDisplayTime;
    Refresh = true;
    ReferenceTime = millis();
  }

  if(Refresh) {
    Refresh = false;
    switch (DisplayMode) {
      case bootScreen:
        lcd.clear();
        lcd.setCursor(0, 0); 
        lcd.print("MultimeterMM2019");
        lcd.setCursor(0, 1);
        lcd.print("Firmware:");
        lcd.setCursor(9, 1);
        lcd.print(FirmwareVersion);
        break;
      case voltageDC:
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("----.---"); 
        lcd.setCursor(12, 0); lcd.print("V");
        lcd.setCursor(13, 0); lcd.print("/");
        lcd.setCursor(14, 0);
        break;
      case fatalErr:
        lcd.clear();
        lcd.setCursor(0, 0); 
        lcd.print("Fatal Error kua!");
        break;
      default:
        break;
    }
  }
  return;
}

void RefreshDisplay(void) {

  switch (DisplayMode) {
    case bootScreen:
      lcd.clear();
      lcd.setCursor(0, 0); 
      lcd.print("MultimeterMM2019");
      lcd.setCursor(0, 1);
      lcd.print("Firmware:");
      lcd.setCursor(9, 1);
      lcd.print(FirmwareVersion);
      break;
    case voltageDC:
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("----.---"); 
      lcd.setCursor(12, 0); lcd.print("V");
      lcd.setCursor(13, 0); lcd.print("/");
      lcd.setCursor(14, 0);
      break;
    case fatalErr:
      lcd.clear();
      lcd.setCursor(0, 0); 
      lcd.print("Fatal Error kua!");
      break;
    default:
      break;
  }

}
