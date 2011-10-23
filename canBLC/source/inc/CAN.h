// Header:
// File Name: CAN.h
// Author: Konstantin Reichmeyer
// Date: 30.04.10



void CAN_Configuration  (void);
uint8_t ConfigMessageOff   (void);
uint8_t ConfigMessageOn    (void);

/*Define names for Virtual addresses stored in: 
/   volatile uint16_t VirtAddVarTab[NumbOfVar] = {flashminpwm, flashmaxpwm, flashaddress, flashangle};*/
#define flashminpwm     0x5555
#define flashmaxpwm     0x6666
#define flashaddress    0x7777
#define flashangle      0x8888

/* Motor stops when there has no CAN speed frame been received for more than TIMEOUT ms  */
#define TIMEOUT         200    //can message timeout in ms  
#define CANLOGprescaler 100    //can log timebase in ms
