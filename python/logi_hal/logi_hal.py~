import fcntl, os, time, struct, binascii, math
import logipi


def setServoPulse(self, address, index, pos):	
		logipi.directWrite(address+(index * 2), ((pos & 0x00FF), ((pos >> 8) & 0x00FF)));
	
def setServoFailSafeAngle(self, address, index, angle, max_pulse=255.0, min_angle=-45.0, max_angle=45.0):
	quanta = max_pulse/(max_angle-min_angle)
        pulse = (max_pulse/2) + (quanta * angle)
        pulse = max(min(int(round(pulse)), 255), 0)
	logipi.directWrite(address+((index * 2) + 1), ((pulse & 0x00FF), ((pulse >> 8) & 0x00FF)));

def setServoAngle(self, address, index, angle, max_pulse=255.0, min_angle=-45.0, max_angle=45.0):
	quanta = max_pulse/(max_angle-min_angle)
        pulse = (max_pulse/2) + (quanta * angle)
	pulse = max(min(int(round(pulse)), 255), 0)
	self.setServoPulse(index, int(round(pulse)))

def setPWMDivider(self, address, divider):	
		logipi.directWrite(address, ((divider & 0x00FF), ((divider >> 8) & 0x00FF)));

def setPWMPeriod(self, address, period):	
		logipi.directWrite(address+1, ((period & 0x00FF), ((period >> 8) & 0x00FF)));

def setPWMPulse(self, address, chan, pulse):	
		logipi.directWrite(((address+2)+chan), ((pulse & 0x00FF), ((pulse >> 8) & 0x00FF)));

def setGPIODir(self, address, dir_mask):	
		logipi.directWrite(address+1, ((dir_mask & 0x00FF), ((dir_mask >> 8) & 0x00FF)));

def setGPIOVal(self, address, val):	
		logipi.directWrite(address, ((val & 0x00FF), ((val >> 8) & 0x00FF)));

def getGPIOVal(self, address):	
		read_val = logipi.directRead(address, 2);
		return read_val[0]+(read_val[1] << 8)

def enableWatchdog(self, address):	
		logipi.directWrite(address, (0x01, 0x00) );

def disableWatchdog(self, address):	
		logipi.directWrite(address, (0x00, 0x00) );

def resetWatchdog(self, address):	
		logipi.directWrite(address, (0x01, 0x00) );
		

	


