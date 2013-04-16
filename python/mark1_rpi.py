import fcntl, os, time, struct, binascii
import spi


write_cmd = 0x00
read_cmd = 0x01

class Mark1Rpi:
	
	def __init__(self):
		spi.openSPI(speed=30000000)

	def read(self, addr, nb, inc):
		transfer_tuple = ()
		cmd = (addr << 2) + read_cmd + + (inc << 1)
		transfer_tuple = transfer_tuple + ((cmd >> 8),)
		transfer_tuple = transfer_tuple + ((cmd & 0x00FF),)
		transfer_tuple = transfer_tuple + tuple([0]*nb)
		data = spi.transfer(transfer_tuple)
		return list(data)[2:]

	def write(self, addr, vals, inc):
		transfer_tuple = ()
		cmd = (addr << 2) + write_cmd + (inc << 1)
		transfer_tuple = transfer_tuple + ((cmd >> 8),)
		transfer_tuple = transfer_tuple + ((cmd & 0x00FF),)
		for v in vals:
			transfer_tuple = transfer_tuple + (v,)
		spi.transfer(transfer_tuple)

	def close(self):
		spi.closeSPI()
	

if __name__ == "__main__":
	mark1 = Mark1()
	try:	
		
	except KeyboardInterrupt:
		print("Terminated by Ctrl+C")
		exit(0)

