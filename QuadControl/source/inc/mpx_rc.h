// Header:    Parses mpx m-link receivers B/D serial servo data
// File Name: mpx_rc.h
// Author:    Konstantin Reichmeyer
// Date:      18.03.2012


void mpx_parse_data (uint8_t); 
void crc_ccitt_update(unsigned char);
unsigned int crc_ccitt_crc(void);
void crc_ccitt_init(void);
