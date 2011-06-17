/* =========================================================================== */
/* Project:        quadcontrol */
/* File:           mixer.h */
/* Organization:    */
/* Author:         Konstantin Reichmeyer */
/* Date:           02/09/2009 */
/* =========================================================================== */

/* Private typedef -----------------------------------------------------------*/
struct pid
{
  int levelerror[2];
  int levellasterror[2];
  int levelerror_int[2];
  int levelsign[2];
  int gyroneutrl[2];
};

struct pdyaw
{
  int yawerror;
  int yawlasterror;
  int yawerror_int;
  int yawsign;
  int gyroneutrl;
};


#define GASLIMIT  20




/* Private function prototypes -----------------------------------------------*/
void mixer(void);
void levelcontroller(int16_t, int);
void yawcontroller(int16_t);
void mode_off(void);
void mode_standby(void);
void mode_flight(void);

