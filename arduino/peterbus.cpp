

#include "peterbus.h"
#include "Arduino.h"

int framepos = 0;
unsigned char id;
unsigned char len;
unsigned char current_len;
unsigned char checksum;
bool escape_character=false; 
bool completed_frame=false;

unsigned char rxbuffer[64];
unsigned char txbuffer[64];
unsigned char txlen;



/*
 * peterbus initialization
 * */
void pb_init()
{
	escape_character=false;
	completed_frame=false;
	framepos=0;
}

/*
 * peterbus shutdown
 **/ 
void pb_close()
{
	
}

void pb_push_rx(unsigned char data)
{
	
	/* frame is reseted */
	if(data==0x7e && framepos!=0)framepos=0;
	
	if(data==0x7d)
	{
		escape_character=true;
		return;
	}
	
	if(escape_character)
	{
		escape_character=false;
		data = data ^ 0x20;
	} 
	
	switch(framepos)
	{
		/* waiting for header */
		case 0:
			completed_frame=false;
			
			if(data==0x7e)
			{
				framepos=1;
			}
		break;
		
		/* id */
		case 1:
			framepos=2;
			id = data;
				
		break;
		
		/* frame size */
		case 2:
			len=data;
			current_len=0;
			framepos=3;
		
		break;
		
		/* frame data */
		case 3:
			if(current_len==0)
			{
				checksum=id ^ len;
			}
			
			checksum=checksum ^ data;
								
			rxbuffer[current_len]=data;
				
				
			current_len++;
			if(current_len==len)
			{
				framepos=4;
			}
			
			 
		break;
		
		case 4:
			if(data==checksum)
			{
				completed_frame=true;
				framepos=0;
				//cout<<"* Frame type "<<hex<<(int)id<<" size "<<(int)len<<endl;
			}
			else
			{
				framepos=0;
				/* checksum does not match */
				//cout<<"* Bad frame data"<<endl;
			}
		break;
	}
	
		

}

bool pb_frame_available()
{
	return completed_frame;
}

void pb_push_tx(unsigned char data)
{
	txbuffer[txlen]=data;
	txlen++;
}

bool pb_read(unsigned char * in_id,unsigned char * in_len,unsigned char * in_buffer)
{
	unsigned char data;
	
	while(Serial.available())
	{
		data = Serial.read();
		
		pb_push_rx(data);
		if(pb_frame_available())
		{
			*in_id=id;
			*in_len=len;
			for(int n=0;n<len;n++)
			{
				in_buffer[n]=rxbuffer[n];
			}
			return true;
		}
	}

	return false;
}

void pb_write(unsigned char out_id,unsigned char out_len,unsigned char * out_buffer)
{
	unsigned char checksum;
	
	txlen=0;
	
	/* frame header */
	pb_push_tx(0x7e);
	
	/* id */
	if(out_id==0x7e || out_id==0x7d)
	{
		pb_push_tx(0x7d);
		pb_push_tx(out_id ^ 0x20);
	}
	else
	{
		pb_push_tx(out_id);  
	}
  
	/* data len */
	if(out_len==0x7e || out_len==0x7d)
	{
		pb_push_tx(0x7d);
		pb_push_tx(out_len ^ 0x20);
	}
	else
	{
		pb_push_tx(out_len);
	}
    
    /* id and len are included in checksum */
	checksum = out_id ^ out_len;
  
	/* data */
	for(int n=0;n<out_len;n++)
	{
   
		checksum=checksum ^ out_buffer[n];
   
		if(out_buffer[n]==0x7e || out_buffer[n]==0x7d)
		{
		  pb_push_tx(0x7d);
		  pb_push_tx(out_buffer[n]^ 0x20);
		}
		else
		{
		  pb_push_tx(out_buffer[n]); 
		}
	 }
  
	/* checksum */
	if(checksum==0x7e || checksum==0x7d)
	{
	pb_push_tx(0x7d);
	pb_push_tx(checksum ^ 0x20);
	}
	else
	{
	pb_push_tx(checksum);
	}
	  
	
	for(int n=0;n<txlen;n++)
	{
		Serial.write(txbuffer[n]);
	}
	
}



PeterBus::PeterBus();
{
	tx_size=0;
	tx_pos=0;
}

void PeterBus::BeginTx(unsigned char id)
{
	tx_size=0;
	tx_pos=3;
	
	/* header */
	tx[0]=0x7e;
	
	/* id */
	tx[1]=id;
	
	/* length (will be filled later) */
	tx[2]=0x00;
	
}

void PeterBus::PushInt8(unsigned char v)
{
	tx[tx_pos]=v;
	tx_pos++;
	tx_size++;
}

void PeterBus::PushInt16(int v)
{
	tx[tx_pos]=0x00FF & v;
	tx[tx_pos+1]=(0xFF00 & v)>>8;
	tx_pos+=2;
	tx_size+=2;
}

void PeterBus::PushInt32(long v)
{
	tx[tx_pos]=0x000000FF & v;
	tx[tx_pos+1]=(0x0000FF00 & v)>>8;
	tx[tx_pos+2]=(0x00FF0000 & v)>>16;
	tx[tx_pos+3]=(0xFF000000 & v)>>24;
	
	tx_pos+=4;
	tx_size+=4;
}

void PeterBus::PushFloat(float v)
{
	long * w = (long *)&v;
	
	tx[tx_pos]=0x000000FF & *w;
	tx[tx_pos+1]=(0x0000FF00 & *w)>>8;
	tx[tx_pos+2]=(0x00FF0000 & *w)>>16;
	tx[tx_pos+3]=(0xFF000000 & *w)>>24;
		
	tx_pos+=4;
	tx_size+=4;
}

int PeterBus::EndTx()
{
	unsigned char checksum = 0;
	unsigned char n;
	
	tx[2]=tx_size;
	
	for(n=0;n<tx_size;n++)
	{
		checksum = checksum ^ tx[3+n];
	}
	
	
	tx[tx_pos]=checksum; /* cheksum here */
}