
#ifndef _PETERBUS_
#define _PETERBUS_

class PeterBus
{
	public:
	
	unsigned char * tx;
	unsigned char * rx;
	int size_tx;
	int size_rx;
	
	PeterBus(int size_tx,unsigned char * buff_tx,int size_rx,unsigned char * buffer_rx);
	
	void BeginTx(unsigned char id);
	void PushInt8(unsigned char v);
	void PushInt16(int v);
	void PushInt32(long v);
	void PushFloat(float v);
	int EndTx();
};

#endif
