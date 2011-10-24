// Header:
// File Name: main.h
// Author: Konstantin Reichmeyer
// Date: 28.04.10


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


#define MINPWM      63
#define MAXPWM      1024          //can only be reduced by can message
#define ADDRESS     0

#define IDLE        600000

#define CURRENTLIMIT 1000

#define flashadress_ADDRESS 0x1000
#define flashadress_MINPWM  0x1001
#define flashadress_MAXPWM  0x1002




void Delay                  (__IO uint32_t nTime);
void TimingDelay_Decrement  (void);

