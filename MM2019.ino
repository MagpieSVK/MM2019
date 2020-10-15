/** Main module of MM2019 project */

#include "gl_hd.h"

uint8_t timer10x;                       ///< 10 x FSyncRefreshTime timer
uint8_t timer100x;                      ///< 100 x FSyncRefreshTime timer
uint8_t UnsychronisedInteruptCnt;       ///< Unsychronised interupt counter
uint8_t FatalErr;                       ///< Fatal Error flag


/**
 * @brief Setup function
 * 
 * Run once setup function. This function Runs once on the startup.
 * 
 * @param void This is void function ut doesnt take any parameter
 * 
 */
void setup(void) {
  // Internal Interupts inicialization
  InicMeassure();
  InicDisplay();
  InitSerial();
  Timer1.initialize(FSyncRefreshTime);
  delay(StartUpDelay);                      ///< We will delay setup to give LCD and other periferies time to initialize 

}

/**
 * @brief Main Loop function
 * 
 * Main function running in loop. In reality this function will be prevented from running
 * in loop. Inside the function FAsync will be called in infinite loop until the FatalErr
 * is not raised. After FatalErr is raised FAsync functions will not be called anymore
 * FSync function will be detached from Interupt and code will go into Err func.
 * 
 * @param void This is void function it doesnt take any parameter
 * 
 */
void loop(void) {

  Timer1.attachInterrupt(FSync);
  while (!FatalErr)
  {
    FAsync();
  }
  Timer1.detachInterrupt();
}

/** 
 * @brief Synchronous function
 *  
 * Functions runned from this function will execute according to FSyncRefreshTime setting
 * This function cannot last longer than FSyncRefreshTime
 * Based on timer10x you can run functions every 10 x FSyncRefreshTime. You should split those functions
 * to different runs i.e when timer10x = 0,1,2, etc. but beware timer will never reach 10!
 * 
 * @param void This is void function it doesnt take any parameter
 *  
 * @todo finish this function
 */
void FSync(void) {

  static bool Sync = false;       // Interupt running flag
  unsigned long RunTime = 0;

  if (Sync) {
    // Previous interupt was still running when this one was called
    // We are taking long to finish functions
    UnsychronisedInteruptCnt++;
    if (UnsychronisedInteruptCnt == MAX_NUMBER_OF_USINT)
      // Lot of interupt calls were unfinished we raise Err
      FatalErr = true;
  }
  // Interupt function is running
  Sync = true;
  
  // functions which run every FSync
  RefreshRange();
  // functions which run every odd FSync
  if(GetFlag(timer10x, 0b00000001)) {
    RefreshVoltage();
  }
  // functions which run every even FSync
  else {
    RefreshCurrent();
  }
  if(timer10x == 8) {
    ReadSerial();
  }
  // functions which run every 10 x FSyncRefreshTime on 9 run of FSync
  if(timer10x == 9) {
    SetDisplay();
  }
  // functions which run every 100 x FSyncRefreshTime on 10 run of FSync and reset of 10x and 100x timers
  // Dont run any 10x functions from here since 100x functions run from here
  if(++timer10x == 10) {
    timer10x = 0;
    if(++timer100x == 10) {
      timer100x = 0;
    }
  }
  // Interupt finished - no code after this
  RunTime = Timer1.read();
  CaptureDiagTime(timer10x, RunTime);
  
  Sync = false;
  return;

}

/** 
 * @brief Asynchronous function
 *  
 * Functions running from this function will run in spare time left
 * by FSync function. Only time non-sensitive functions should run from
 * this function.
 * 
 * @param void This is void function it doesnt take any parameter
 *  
 * @todo finish this function
 */
void FAsync(void) {
   //RefreshDisplay();
   WriteSerial();
   ComputeDiagTime();
  return;
}