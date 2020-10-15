/** serial module definitions */
void InitSerial(void);
void ReadSerial(void);
void WriteSerial(void);
void SendFSyncDiagnostic(void);
void SendProductDetails(void);
void CaptureDiagTime(int FSync, unsigned long Timer);
void ComputeDiagTime(void);
void DebugTimeStart(unsigned long Timer);
void DebugTimeEnd(unsigned long Timer);

/** struct definitions */

typedef struct {
    bool exec;
    void (*WriteFunction)();
} stWriteFuncExec;

enum enmWriteFunction {
    AboutWrite,
    FSyncDiag,
    EndWrite
};

///< FSyncDiagnostic Struct
typedef struct {
    uint8_t  MissingSync;       ///< How many FSync run has failed
    uint16_t AverageDebug;      ///< Debuged section run time [us]
    uint16_t MaximumDebug;      ///< Maximum time of FSync [us]
    uint16_t MinimumDebug;      ///< Minimum time of FSync [us]
    uint16_t FSync[10];         ///< Time of give FSync run [us]
} stFSyncDiagnostic;