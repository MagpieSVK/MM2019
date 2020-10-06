/** Standard arduino includes */

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <EEPROM.h>

/** Special arduino includes*/
#include <TimerOne.h>

/** Module includes */
#include "meassure.h"
#include "diag.h"

/** Global definitions */

#define us_in_s 1000000
#define ms_in_s 1000

#define FSyncRefreshTime 10000                  ///< FSyncRefreshTime in microseconds
#define MAX_NUMBER_OF_USINT 0xFF                ///< Maximal number of times the FSyncFunction can fail to run properly

/** Globaly usable macros */
#define GetFlag(var,msk) (var&msk)
#define NGetFlag(var,msk) (!(var&msk))
#define SetFlag(var,msk) var|msk
#define ClearFlag(var,msk) var&(!msk)