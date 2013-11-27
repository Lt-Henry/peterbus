
#ifndef _PETERBUS_
#define _PETERBUS_



void pb_init();
void pb_close();

bool pb_read(unsigned char * id,unsigned char * len,unsigned char * in_buffer);
void pb_write(unsigned char id,unsigned char len,unsigned char * out_buffer);
	
void pb_push_rx(unsigned char data);
void pb_push_tx(unsigned char data);
bool pb_frame_available();

#endif
