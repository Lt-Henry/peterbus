
#ifndef _PETERBUS_
#define _PETERBUS_

class PeterBus
{
	public:
	
	unsigned char  tx[28];
	unsigned char  rx[28];
	unsigned char tx_size;
	unsigned char tx_pos;
	
	unsigned char tx_pop;
	bool tx_escape;
	
	unsigned char rx_size;
	unsigned char rx_pos;
	
	bool rx_escape;
	
	PeterBus();
	
	void BeginTx(unsigned char id);
	void PushInt8(unsigned char v);
	void PushInt16(int v);
	void PushInt32(long v);
	void PushFloat(float v);
	void EndTx();
	
	bool IsTxAvailabe();
	unsigned char PopTx();
	
	void PushRx(unsigned char v);
	bool IsRxFrame();
};

#endif
