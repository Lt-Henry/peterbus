

import PeterBus
import serial

pbus = PeterBus.PeterBus()
s = serial.Serial("/dev/ttyUSB0",9600)

pbus.begin_tx(0x23)
pbus.push_int8(0x76)
pbus.push_int8(0x67)
pbus.end_tx()

while(pbus.is_tx_available()):
	d = pbus.pop_tx()
	s.write(chr(d).encode())


s.close()
