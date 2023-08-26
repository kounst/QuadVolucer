/*
    Copyright (c) 2008 Stefan Engelke <stefan@tinkerer.eu>

    Permission is hereby granted, free of charge, to any person 
    obtaining a copy of this software and associated documentation 
    files (the "Software"), to deal in the Software without 
    restriction, including without limitation the rights to use, copy, 
    modify, merge, publish, distribute, sublicense, and/or sell copies 
    of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be 
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.
    
    $Id: rcdsl.c 60 2008-08-21 07:50:48Z taser $
    
    RCDSL.H and RCDSL.C is an INOFFICIAL implementation of the 
    communication protocol used by DSL receivers of Act Europe. 
    The DSL receivers have a serial communication port to connect 
    two receivers in diversity mode. Each receiver is sending the 
    received servo signals periodically over this port. This fact 
    can be used to connect the receiver to the control unit of the 
    model via UART instead of evaluating the PPM signal. 
    The UART parameters are
    38400 Baud, 8-N-1
    
    If you have any questions, fell free to send me an e-mail.

*/


#include "stm32f10x.h"
#include "main.h"
#include "rcdsl.h"

u8 data_counter   = 0;
u8 last_byte      = 0;
u8 check_sum      = 0;
u8 paket_len      = 0;


extern u8 rcdsl_battery;
extern u8 rcdsl_allocation;
extern u8 rcdsl_RSSI;
extern union pulsw pulswidth;
u8 data[6];


typedef union {
        u16 pos[2];
        u8 dat[4];
} servos_t;



void rcdsl_new_signal(u8 servo, int signal) 
// This function is called, when a new servo signal is properly received.
// Parameters: servo  - servo number (0-9)
//             signal - servo signal between 7373 (1ms) and 14745 (2ms)
{
    // *** IMPLEMENT CUSTOM CODE TO REACT TO NEW SERVO SIGNALS IN THIS FUNCTION ***
    // As an example the following code sets SenderOkay, PPM_diff and PPM_in
    // for use with the www.mikrokopter.com project.
    
          
    //signal-= 11059;             // Neutralstellung zur Null verschieben
    
    pulswidth.pw[servo] = signal;
    
}


void rcdsl_incoming_paket() 
// This function is called within rcdsl_parse_data(), when a complete 
// data paket with matching checksum has been received.
{
    
    u8  i;
    static servos_t servos;

    // Look for status headers
    if ((data[0])==0x1F) {
        // Get frequency allocation
        rcdsl_allocation = data[0+1];
        // Get signal quality
        rcdsl_RSSI = data[2+1];
        // Get voltage of battery supply
        rcdsl_battery = data[3+1];
    }
  
    //if(rcdsl_RSSI)
    {

      // Look for s  ignal headers
      if ((data[0]&0xF0)==0x10)
      {
      
          i = data[0]&0x0F;   // Last 4 bits of the header indicates servo pair

          if (i<10)
          {
              // Convert byte array to two uint16
              servos.dat[1] = data[1];
              servos.dat[0] = data[2];
              servos.dat[3] = data[3];
              servos.dat[2] = data[4];
              
              rcdsl_new_signal(i  ,  (u16)servos.pos[0]);
              rcdsl_new_signal(i+1,  (u16)servos.pos[1]);
          }
      }
    }

}


void rcdsl_parse_data(u8 b) 
// This function should be called externaly, when a new data byte has 
// been received over uart.
{

    // Check for sync condition
    if ((b==0xFF) && (last_byte==0xFF)) {
        data_counter = 0;
        check_sum = 0;
        return;
    }
    
    // First byte is cmd
    if (data_counter == 0) {
        if (b==0x1F) paket_len = 5; else paket_len=4;
    } 

    // Last byte is checksum
    if (data_counter > paket_len) {
        
        // Calculate checksum
        check_sum = ~check_sum;
        if (check_sum==0xFF) check_sum=0xFE;

        // If it match the received one, then apply data
        if (b==check_sum) rcdsl_incoming_paket();

        // Prepare for a new data paket
        data_counter = 0;
        check_sum = 0;


    // New byte within a paket
    } else {

        data[data_counter]  = b;
        check_sum          += b;
        data_counter++;

    }
            
    // Remember last byte received for detection of sync condition
    last_byte = b;
}
