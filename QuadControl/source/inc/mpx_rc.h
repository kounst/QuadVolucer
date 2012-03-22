// Header:    Parses mpx m-link receivers B/D serial servo data
// File Name: mpx_rc.h
// Author:    Konstantin Reichmeyer
// Date:      18.03.2012

/*
    Multiplex M-LINK receivers (e.g. RX-7-DR) can be configured to provide digital servo data over a UART interface.
    mpx_rc.h and mpx_rc.c is an implementation of that interface. 
    
    The UART parameters are 115200 Baud, 8-N-1 
*/


void mpx_parse_data (uint8_t); 
uint16_t crc_xmodem_update(uint16_t, uint8_t);
