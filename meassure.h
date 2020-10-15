/* Header for Meassure.cpp module */

/** Function prototypes */
extern void InicMeassure(void);
extern void RefreshVoltage(void);
extern void RefreshCurrent(void);
extern void RefreshRange(void);
extern bool ReturnVoltage(float *pVoltage);
extern bool ReturnVoltage(float *pCurrent);
extern void MeassuredQTypeSelect(bool ACDC);


/** Definitions */
#define VRange400mV 1           ///< 400mV range for range select functions
#define VRange40V 2             ///< 40V range for range select functions
#define VRange400V 3            ///< 400V range for range select functions

#define Voltage400mV 0.400      ///< float definition of 400mv for comparsion
#define Voltage40V 40.0         ///< float definition of 40V for comparsion
#define Voltage400V 400.0       ///< float definition of 400V for comparsion

#define V04 NULL                ///< IO for 400mV range selection (unused this range is always active)
#define V40 4                   ///< IO for 40V range selection
#define V400 5                  ///< IO for 400V range selection

/** Asynchronous lock flags for frequency gathering*/
#define FreqVExtLock 0x01       ///< External lock of voltage frequency - external function is reading data interrupt function cannot write
#define FreqAExtLock 0x02       ///< External lock of current frequency - external function is reading data interrupt function cannot write
#define FreqVIntLock 0x04       ///< Internal lock of voltage frequency - external function cannot gather data durring interrupt writing
#define FreqAIntLock 0x08       ///< Internal lock of current frequency - external function cannot gather data durring interrupt writing


#define VoltageADC_CS 9         ///< IO connected to CS of ADC for Voltage reading
#define CurrentADC_CS 10        ///< IO connected to CS of ADC for Current reading

#define CurrentACDC_Sel 8       ///< IO to select between AC and DC meassuring of current
#define VoltageACDC_Sel 7       ///< IO to select between AC and DC meassuring of voltage

#define AC true                 ///< Definition of AC for Meassure type selection
#define DC false                ///< Definition of DC for Meassure type selection


/** Calibration Definitions */
/**
 * @todo Some of this should be setupble from Serial Comm. Maybe some new module should be created in future
 * 
*/
#define N 16777216.0
#define Nhalf 8388608

#define Vref 5.000
#define R 1995.00
#define R400 ((1000 * 0.86)/(1000 + 0.86))  
#define R40 ((1000 * 9.96)/(1000 + 9.96))
#define R04 1000.00
#define Null 0.00
  
#define R0 ((R04) / (R04 + R))          
#define R1 ((R40) / (R40 + R))
#define R2 ((R400) / (R400 + R))
  
#define CurrentShuntValue 0.100            ///< Move to EEPROM
#define GainOfVoltageAmplifier 10.09  
#define GainOfCurrentAmplifier 5.02
  
#define BT1 A7
#define BT2 A6
  
#define VCross 3
#define ACross 2

#define SlopeV04 (1.0/(R0*N*GainOfVoltageAmplifier))      
#define SlopeV40 (1.0 / (R1*N*GainOfVoltageAmplifier))
#define SlopeV400 (1.0 / (R2*N*GainOfVoltageAmplifier))
#define SlopeA4 (1.0 / (CurrentShuntValue*N*GainOfCurrentAmplifier))
#define OffsetV04 0.01634
#define OffsetV40 0.01486
#define OffsetV400 0.00
#define calSlopeV04 0.99547
#define calSlopeV40 0.99944
#define calSlopeV400 0
#define calSlopeA4 0