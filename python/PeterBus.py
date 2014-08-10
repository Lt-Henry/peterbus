
class PeterBus:

	def __init__(self):
		self.tx = [0] * 28
		self.ty = [0] * 28

		self.tx_size=0
		self.tx_pos=0
		
		self.tx_escape=False
		self.tx_pop=0


	def begin_tx(self,id):
		self.tx[0]=0x7e
		self.tx[1]=id
		self.tx[2]=0
		
		self.tx_size=0
		self.tx_pos=3
		
		
	def push_int8(self,value):
		self.tx[self.tx_pos]=value
		
		self.tx_size=self.tx_size+1
		self.tx_pos=self.tx_pos+1
		
		
	def end_tx(self):
		
		self.tx[2]=self.tx_size
		checksum = self.tx[1] ^ self.tx[2]
		
		for n in range(self.tx_size):
			checksum = checksum ^ self.tx[3+n]
			
		self.tx[self.tx_pos]=checksum
		
		self.tx_escape=False
		self.tx_pop=0



	def is_tx_available(self):
		return (self.tx_pop<=self.tx_pos)
		
		
		
		
	def pop_tx(self):
				
		#header
		if(self.tx_pop==0):
			tmp=self.tx[self.tx_pop]
			self.tx_pop=self.tx_pop+1
			return tmp
			
		#conventional data
		else:
			
			if(self.tx_escape==True):
				tmp=self.tx[self.tx_pop]
				self.tx_pop=self.tx_pop+1
				self.tx_escape=False
				return 0x20 ^ tmp
				
			else:
				
				tmp=self.tx[self.tx_pop]
				
				if(tmp==0x7d or tmp==0x7e):
					self.tx_escape=True
					return 0x7d
				else:
					self.tx_pop=self.tx_pop+1
					return tmp
