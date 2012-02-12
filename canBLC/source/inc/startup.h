// Header:
// File Name: startup.h
// Author: Konstantin Reichmeyer
// Date: 28.04.10

#define POSITIONING_FORCE      50
#define START_PUSH_FORCE       100
#define START_RAMP_FORCE       120          //has to be adapted to supply voltage
#define RAMP_INITIAL_PERIOD 40000
void forced_start_push  (void);  //mode 0
void forced_start_ramp  (void);  //mode 1
void positioning        (void);




uint8_t startup            (uint8_t mode);

