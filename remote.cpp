
#include "gl_hd.h"

stFSyncDiagnostic FsyncDiagnostic;

static uint16_t DebugTime;

stWriteFuncExec WriteFuncExec[] = {
  {false, SendProductDetails},
  {false, SendFSyncDiagnostic}
};

void InitSerial(void) {

  uint8_t i;

  Serial.begin(9600);
  FsyncDiagnostic.MissingSync = 0x00;
  FsyncDiagnostic.MaximumDebug = 0x0000;
  FsyncDiagnostic.MinimumDebug = 0xFFFF;
  FsyncDiagnostic.AverageDebug = 0x0000;
  for(i = 0; i<10; i++) {
    FsyncDiagnostic.FSync[i] = 0x0000;
  };
  return;
}

void ReadSerial(void) {
  char empty;
  char prefix[4];
  char command[4];
  if(Serial.available()) {
    if((Serial.peek()) == '_') {
      Serial.readBytes(&empty, 1);
      Serial.readBytes(prefix, 3);
      prefix[3] = 0;
      Serial.readBytes(&empty, 1);
      Serial.readBytes(command, 3);
      command[3] = 0;
      Serial.readBytes(&empty, 1);
      if(!strcmp(prefix, "GET")){
        if(!strcmp(command, "ABT")) {
          WriteFuncExec[AboutWrite].exec = true;
        }
        if(!strcmp(command, "FDG")) {
          WriteFuncExec[FSyncDiag].exec = true;  
        }
        if(empty == ',')
          ReadSerial();
      }
      else if(prefix == "SET") {
      }
    }
    else {
      Serial.flush();
    }
  }
  return;
} 

void WriteSerial(void) {

  static uint8_t i;

  if(i == EndWrite)
    i = 0;
  if(WriteFuncExec[i].exec == true) {
    WriteFuncExec[i].WriteFunction();
    WriteFuncExec[i].exec = false;
  }
  i++;
  return;
}

void SendFSyncDiagnostic(void) {

  uint8_t i;

  Serial.println("FSync Time diagnostics");
  Serial.print("\tMisingSync: ");
  Serial.println(FsyncDiagnostic.MissingSync);
  Serial.print("\tAverageDebug: ");
  Serial.println(FsyncDiagnostic.AverageDebug);
  Serial.print("\tMaximumDebug: ");
  Serial.println(FsyncDiagnostic.MaximumDebug);
  Serial.print("\tMinimumDebug: ");
  Serial.println(FsyncDiagnostic.MinimumDebug);
  Serial.println("\tIndividualFsynctimes: ");
  for(i = 1; i<=10; i++) {
    Serial.print("\t\tFSync[");
    Serial.print(i);
    Serial.print("]:");
    Serial.println(FsyncDiagnostic.FSync[i-1]);
  };
  Serial.println("Diagnostics done!");
  return;
}

void SendProductDetails(void) {

  Serial.println("Multipurpose Multimeter MM2019");
  Serial.println("Designed by Rastislav Straka, Jakub Vadovsky and Aurel Skoda");
  Serial.print("Firmware version:");
  Serial.println(FirmwareVersion);
  Serial.print("Compiled:");
  Serial.print(CompileTime);
  Serial.print(" ");
  Serial.println(CompileDate);

  return;
}

void CaptureDiagTime(int FSync, unsigned long Timer) {

  FsyncDiagnostic.FSync[FSync] = ((FsyncDiagnostic.FSync[FSync] + (uint16_t)Timer) / 2);
  return;

}

void ComputeDiagTime(void) {

  static uint8_t i; 
  uint16_t Average;

  FsyncDiagnostic.MissingSync = UnsychronisedInteruptCnt;
  Average = FsyncDiagnostic.FSync[0];
  for(i = 0; i<10; i++) {
    Average = ((Average + FsyncDiagnostic.FSync[i]) / 2);
  };

  return;
}

void DebugTimeStart(unsigned long Timer) {
  
  DebugTime = uint16_t(Timer);
  return;

}

void DebugTimeEnd(unsigned long Timer) {

  DebugTime = (uint16_t)Timer - DebugTime;
  FsyncDiagnostic.AverageDebug = ((FsyncDiagnostic.AverageDebug + DebugTime) / 2);
  FsyncDiagnostic.MaximumDebug = max(FsyncDiagnostic.MaximumDebug, DebugTime);
  FsyncDiagnostic.MinimumDebug = min(FsyncDiagnostic.MinimumDebug, DebugTime);
  
  return;
}