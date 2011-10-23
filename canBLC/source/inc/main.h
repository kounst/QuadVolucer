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



void Delay                  (__IO uint32_t nTime);
void TimingDelay_Decrement  (void);

