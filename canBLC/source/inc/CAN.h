// Header:
// File Name: CAN.h
// Author: Konstantin Reichmeyer
// Date: 30.04.10



void CAN_Configuration  (void);
uint8_t ConfigMessageOff   (void);
uint8_t ConfigMessageOn    (void);

/*Define names for Virtual addresses stored in: 
/   volatile uint16_t VirtAddVarTab[NumbOfVar] = {flashminpwm, flashmaxpwm, flashaddress, flashangle, flashcantimeout};*/
#define flashminpwm     0x1001
#define flashmaxpwm     0x1002
#define flashaddress    0x1003
#define flashangle      0x1004
#define flashcantimeout 0x1005

/* Motor stops when there has no CAN speed frame been received for more than TIMEOUT ms  */
#define TIMEOUT         10000    //can message timeout in ms  
#define CANLOGprescaler 100     //can log timebase in ms
