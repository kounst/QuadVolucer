// Header:
// File Name: startup.h
// Author: Konstantin Reichmeyer
// Date: 28.04.10

#define POSITIONING_FORCE      60
#define START_PUSH_FORCE      200
#define START_RAMP_FORCE      200
#define RAMP_INITIAL_PERIOD 30000
void forced_start_push  (void);  //mode 0
void forced_start_ramp  (void);  //mode 1
void positioning        (void);




void startup            (uint8_t mode);

