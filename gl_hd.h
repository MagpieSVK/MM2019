/** Standard arduino includes */

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <EEPROM.h>

/** Special arduino includes*/
#include <TimerOne.h>

/** Module includes */
#include "meassure.h"
#include "display.h"
#include "remote.h"
#include "control.h"

/** Global variables */
extern uint8_t timer10x;                       ///< 10 x FSyncRefreshTime timer
extern uint8_t timer100x;                      ///< 100 x FSyncRefreshTime timer
extern uint8_t UnsychronisedInteruptCnt;       ///< Unsychronised interupt counter
extern uint8_t FatalErr;                       ///< Fatal Error flag

/** Global definitions */
#define FirmwareVersion "0.0.1a"
#define CompileTime __TIME__
#define CompileDate __DATE__

#define StartUpDelay 2000                       ///< Delay at startup in miliseconds

#define us_in_s 1000000
#define ms_in_s 1000

#define FSyncRefreshTime 10000                  ///< FSyncRefreshTime in microseconds
#define MAX_NUMBER_OF_USINT 0xFF                ///< Maximal number of times the FSyncFunction can fail to run properly

/** Globaly usable macros */
#define GetFlag(var,msk) (var&msk)
#define NGetFlag(var,msk) (!(var&msk))
#define SetFlag(var,msk) var|msk
#define ClearFlag(var,msk) var&(!msk)
