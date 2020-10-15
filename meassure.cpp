/* Meassure module */

#include "gl_hd.h"

static float Voltage;           ///< Meassured Voltage
static float Current;           ///< Meassured Current
static bool QType;              ///< True = AC False = DC

static bool VoltageStatus;      ///< Vallidity of Meassured Voltage
static bool CurrentStatus;      ///< Vallidity of Meassured Current
static bool AutoVRange;         ///< True = Auto range enabled, False = Auto range disabled
static int VRange;              ///< Voltage range 1=400mV 2=40V 3=400V

static uint8_t AsyncLock;       ///< Asynchronous lock for calculating frequencies
static uint32_t FreqCntV[2];    ///< Voltage Frequency time1 and time 2
static uint32_t FreqCntA[2];    ///< Current Frequency time1 and time 2

bool ADC_Read(int32_t *pData, int CS) {

  uint8_t ADCRaw[4];
  int32_t ADCCode;
  boolean ADCBool;

  /* Enable given chip */
  digitalWrite(CS, LOW);
  /* Read state of EOC */
  ADCBool = digitalRead(MISO);
  if(ADCBool)
    return false;
  /* Begin SPI communication */
  SPI.beginTransaction(SPISettings(350000, MSBFIRST, SPI_MODE1));
  /* Transfer 32 bites from ADC */
  SPI.transfer(ADCRaw, 4);
  /* End SPI communication */
  SPI.endTransaction();
  /* Disable given chip */
  digitalWrite(CS, HIGH);

  /* Mask and find sign byte */
  ADCBool = (ADCRaw[0] & B00100000) >> 5;

  /* Mask Intermetiate bajts */
  ADCRaw[3] = ADCRaw[3] & B11100000;
  ADCRaw[0] = ADCRaw[0] & B00011111;

  /* Concacenate bytes into QWORD*/
  ADCCode = (long) ADCRaw[3] >> 5;
  ADCCode = ADCCode + ((long) ADCRaw[2] << 3);
  ADCCode = ADCCode + ((long) ADCRaw[1] << 11);
  ADCCode = ADCCode + ((long) ADCRaw[0] << 19);

  /* write sign */
  if (ADCBool)
    ADCCode = ~ADCCode;
    ADCCode -= 1; 
  /* write output data */
  *pData = (int32_t)ADCCode;

  return true;
}

void RefreshVoltage(void) {

  int32_t ADCValue;
  float ABSVoltage;

  if(ADC_Read(&ADCValue, VoltageADC_CS)) {
    switch(VRange) {
    case VRange400mV:
      Voltage = (float)ADCValue * Vref * SlopeV04;
      Voltage = (Voltage - OffsetV04) / calSlopeV04;
      ABSVoltage = fabs(Voltage);
      if (AutoVRange) {
        if (ABSVoltage > Voltage400mV)
          VRange++;
          VoltageStatus = false;
          break;
      }
      VoltageStatus = true;
      break;
    case VRange40V:
      Voltage = (float)ADCValue * Vref * SlopeV40;
      Voltage = (Voltage - OffsetV40) / calSlopeV40;
      if (AutoVRange) {
        if (ABSVoltage > Voltage40V) {
          VRange++;
          VoltageStatus = false;
          break;
        }
        else if (ABSVoltage < Voltage400mV) {
          VRange--;
          VoltageStatus = false;
          break;
        }
      }
      VoltageStatus = true;
      break;
    case VRange400V:
      Voltage = (float)ADCValue * Vref * SlopeV400;
      Voltage = (Voltage - OffsetV400);
      if (AutoVRange) {
        if (ABSVoltage < Voltage40V) {
          VRange--;
          VoltageStatus = false;
          break;
        }
      }
      VoltageStatus = true;
      break;
    default:
      break;
    }
  }

  return;
}

void RefreshCurrent(void) {

  int32_t ADCValue;

  if(ADC_Read(&ADCValue, CurrentADC_CS)) {
    if(!QType)Current = (float)ADCValue * Vref * SlopeA4;
    else{
      ADCValue = (Nhalf)-abs(ADCValue);  
      Current = (float)ADCValue * Vref * SlopeA4;
    }
  }
  return;
}

void RefreshRange(void) {
  
  switch(VRange) {
  case VRange400mV:
    digitalWrite(V400, LOW);
    digitalWrite(V40, LOW);
    break;
  case VRange40V:
    digitalWrite(V400, LOW);
    digitalWrite(V40, HIGH);  
    break;
  case VRange400V:
    digitalWrite(V400, HIGH);
    digitalWrite(V40, LOW);    
    break;
  default:
    break;
  }
  return;
}

void Range(int value) {

  if (value == 1) {
    digitalWrite(V400, LOW);
    digitalWrite(V40, HIGH);
  }
  if (value == 2) {
    digitalWrite(V400, HIGH);
    digitalWrite(V40, LOW);
  }
  if (value == 0) {
    digitalWrite(V400, LOW);
    digitalWrite(V40, LOW);
  }
  return;
}


bool FreqResolveV(float *pFreqV) {
  // Variable definitions
  float Temporary;

  SetFlag(AsyncLock, FreqVExtLock);
  if(GetFlag(AsyncLock, FreqVIntLock)) {
    return false;
  }
  Temporary = us_in_s / (abs(FreqCntV[0] - FreqCntV[1]));
  ClearFlag(AsyncLock, FreqVExtLock);
  *pFreqV = Temporary;
  return true;
}

bool FreqResolveA(float *pFreqA) {
  // Variable definitions
  float Temporary;

  SetFlag(AsyncLock, FreqAExtLock);
  if(GetFlag(AsyncLock, FreqAIntLock)) {
    return false;
  }
  Temporary = us_in_s / (abs(FreqCntA[0] - FreqCntA[1]));
  ClearFlag(AsyncLock, FreqAExtLock);
  *pFreqA = Temporary;
  return true;
}

/* This function is not usable */
bool PhaseResolve(float *pPhase) {
  // Variable definitions
  float Temporary;

  SetFlag(AsyncLock, FreqVExtLock|FreqAExtLock);
  if(GetFlag(AsyncLock, FreqVIntLock|FreqAIntLock)) {
    return false;
  }
  Temporary = us_in_s / (abs(FreqCntA[0] - FreqCntA[1]));
  ClearFlag(AsyncLock, FreqVExtLock|FreqAExtLock);
  *pPhase = Temporary;
  return true;
}


void FreqCaptureV(void) {
  // Variable definitions
  static uint8_t Run = 0;

  if(NGetFlag(AsyncLock, FreqVExtLock) || GetFlag(AsyncLock, FreqVIntLock)) {
    SetFlag(AsyncLock, FreqVIntLock);
    if(Run == 0) {
      FreqCntV[0] = micros();
      Run++;
    }
    else {
      FreqCntV[1] = micros();
      ClearFlag(AsyncLock, FreqVIntLock);
      Run = 0; 
    }
  }
  return;
}

void FreqCaptureA(void) {
  // Variable definitions
  static uint8_t Run = 0;

  if(NGetFlag(AsyncLock, FreqAExtLock) || GetFlag(AsyncLock, FreqAIntLock)) {
    SetFlag(AsyncLock, FreqAIntLock);
    if(Run == 0) {
      FreqCntA[0] = micros();
      Run++;
    }
    else {
      FreqCntA[1] = micros();
      ClearFlag(AsyncLock, FreqAIntLock);
      Run = 0; 
    }
  }
  return;
}

bool ReturnVoltage(float *pVoltage) {

  if(VoltageStatus == true)
    *pVoltage = Voltage;
  else
    return false;
  return true;
}

bool ReturnCurrent(float *pCurrent) {

  if(CurrentStatus == true)
    *pCurrent = Current;
  else
    return false;
  return true;
}

void MeassuredQTypeSelect(bool ACDC) {

  digitalWrite(VoltageACDC_Sel, ACDC);
  digitalWrite(CurrentACDC_Sel, !ACDC);
  QType = ACDC;
  return;
}

void InicMeassure(void) {

  // Initialize IO
  pinMode(V40, OUTPUT);
  pinMode(V400, OUTPUT);
  pinMode(VoltageADC_CS, OUTPUT);
  pinMode(CurrentADC_CS, OUTPUT);
  pinMode(VoltageACDC_Sel, OUTPUT);
  pinMode(CurrentACDC_Sel, OUTPUT);
  pinMode(VCross, INPUT_PULLUP);
  pinMode(ACross, INPUT_PULLUP);
  // Set IO
  digitalWrite(V400, LOW);
  digitalWrite(V40, LOW);
  digitalWrite(VoltageADC_CS, HIGH);
  digitalWrite(CurrentADC_CS, HIGH);
  digitalWrite(VoltageACDC_Sel, DC);
  digitalWrite(CurrentACDC_Sel, (!DC));
  // Initialize variables
  Voltage = 0.0;
  Current = 0.0;
  QType = false;
  VoltageStatus = false;
  CurrentStatus = false;
  AutoVRange = true;
  VRange = VRange400mV;
  // Initialize interrupts
  attachInterrupt(digitalPinToInterrupt(VCross), FreqCaptureV, RISING);
  attachInterrupt(digitalPinToInterrupt(ACross), FreqCaptureA, RISING);
  // Init SPI
  SPI.begin();

  return;
}