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
    
    $Id: rcdsl.h 60 2008-08-21 07:50:48Z taser $
    
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




// Received signal strength indicator 
extern u8 rcdsl_RSSI;

// Battery voltage (0-255 [0V - 8.2V])
extern u8 rcdsl_battery;

// Frequency allocation (35,40,72)
extern u8 rcdsl_allocation;

// This function should be called when a new data byte has 
// been received over uart.
void rcdsl_parse_data (u8); 
