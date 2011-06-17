// Header:
// File Name: main.h
// Author: Konstantin Reichmeyer
// Date: 28.04.10


void TimingDelay_Decrement  (void);

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

#define TxBufferSize   (countof(TxBuffer) - 1)
#define countof(a)   (sizeof(a) / sizeof(*(a)))

#define LOWBAT  1966          //9V  1V == 218,4905

#define NEUTRAL_ROTATE_ROLL     11059
#define NEUTRAL_ROTATE_PITCH    11059
#define NEUTRAL_ROTATE_YAW      11059
#define NEUTRAL_ROTATE_THROTTLE 8300

/*Define names for Virtual addresses stored in: 
/   volatile uint16_t VirtAddVarTab[NumbOfVar] = {neutral_pw1, neutral_pw2, neutral_pw3, neutral_pw4}; */
#define neutral_pw1     0x5555
#define neutral_pw2     0x6666
#define neutral_pw3     0x7777
#define neutral_pw4     0x8888


union pulsw
{
  struct pwm
  {
    vu16 pw1;
    vu16 pw2;
    vu16 pw3;
    vu16 pw4;
    vu16 pw5;
    vu16 pw6;
    vu16 pw7;
    vu16 pw8;
    vu16 pw9;
  }puls;
  vu16 pw[9];
};

struct props
{
  u8 front;
  u8 left;
  u8 right;
  u8 rear;
};


/**
  *channels structure indicates the rotation speed. -2000 .. 2000. 
  *It is used to calculate the elements of the gas structure using pulswidth
  */
union channels
{
  struct chan
  {
		int pitch;
	  int roll;
		int yaw;
		int throttle;
  }rotate;
  int level[4];
};
