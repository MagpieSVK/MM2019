/** Header for display.cpp module */

void InicDisplay(void);
void SetDisplay(void);
void RefreshDisplay(void);

enum display_mode {
    bootScreen,
    voltageDC,
    voltageAC,
    currentDC,
    currentAC,
    remoteControl,
    fatalErr,
};