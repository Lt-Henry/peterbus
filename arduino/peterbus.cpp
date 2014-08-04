

#include "peterbus.h"
#include "Arduino.h"



PeterBus::PeterBus();
{
	tx_size=0;
	tx_pos=0;
	
	rx_pos=0;
	rx_size=0;
	
	rx_status=0;
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
	
	tx[2]=tx_size; /* size stored here */ 
	
	for(n=0;n<tx_size;n++)
	{
		checksum = checksum ^ tx[3+n];
	}
	
	
	tx[tx_pos]=checksum; /* checksum stored here */
	
	tx_escape=false;
	tx_pop=0;
}

bool PeterBus::IsTxAvailable()
{
	return(tx_pop<=tx_pos);
}

unsigned char PeterBus::PopTx()
{
	unsigned char tmp;
	
	/* header */
	if(tx_pop==0)
	{
		tmp=tx[tx_pop];
		tx_pop++;
		return tmp;
	}
	else /* conventional data */
	{
		if(tx_escape==true)
		{
			tmp=tx[tx_pop];
			tx_pop++;
			tx_escape=false;
			return 0x20 ^ tmp;
		}
		else
		{
			tmp=tx[tx_pop];
				
			if(tmp==0x7d || tmp==0x7e)
			{
				tx_escape=true;
				return 0x7d;
			}
		}
	}
}

void PeterBus::PushRx(unsigned char v)
{
	if(rx_state!=0)
	{
		/* unexpected header, reset state */
		if(v==0x7e)
		{
			rx_state=0;
			rx_pos=0;
		}
		
		/* escape character */
		if(v==0x7d)
		{
			rx_escape=true;
			return;
		}
	}
	
	if(rx_escape)
	{
		v = v * 0x20;
	}
	
	switch(rx_state)
	{
		/* expecting a header */
		case 0:
			if(v==0x7e)
			{
				rx_size=0;
				rx_pos=0;
				
				rx_state=1;
				rx[rx_pos]=v;
				rx_pos++;
			}
		break;
		
		/* expecting a id */
		case 1:
			rx_state=2;
			rx[rx_pos]=v;
			rx_pos++;
		break;
		
		/* expecting size */
		case 2:
			rx_state=3;
			rx[rx_pos]=v;
			rx_pos++;
			
			/* maybe we should check for invalid frame sizes */
			
			rx_checksum=rx[1] ^ rx[2]; /* init checksum with id and size */
			
		break;
		
		/* expecting data */
		case 3:
			rx_checksum = v ^ rx_checksum;
			rx[rx_pos]=v;
			rx_pos++;
			rx_size++;
			
			if(rx_size==rx[2])
			{
				rx_state=4;
			}
		
		break;
		
		/* expecting checksum */
		case 4:
			rx[rx_pos]=v;
						
			rx_state=0;			
			
		break;
	}
	
}

bool PeterBus::IsRxFrame()
{
	return (rx[2+rx_size+1]==rx_checksum);
	
}

unsigned char PeterBus::BeginRx()
{
	rx_pop=3;
	return rx[1]; /* msg id */
}

unsigned char PeterBus::PopInt8()
{
	unsigned char tmp;
	tmp = rx[rx_pop];
	
	rx_pop++;
	return tmp;
}

int PeterBus::PopInt16()
{
	int * tmp;
	
	tmp = (int *)(rx + rx_pop);
	
	rx_pop+=2;
	return *tmp;
}

long PeterBus::PopInt32()
{
	long * tmp;
	
	tmp = (long *)(rx + rx_pop);
	
	rx_pop+=4;
	return *tmp;

}

float PeterBus::PopFloat()
{
	float * tmp;
	
	tmp = (float *)(rx + rx_pop);
	
	rx_pop+=2;
	return *tmp;

}

void PeterBus::EndRx()
{
}
