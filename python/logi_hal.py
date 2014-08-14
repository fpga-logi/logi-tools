import fcntl, os, time, struct, binascii, math
import logi



def setServoPulse(address, index, pos):	
		logi.logiWrite(address+(index * 2), ((pos & 0x00FF), ((pos >> 8) & 0x00FF)));

def setServoFailSafePulse(address, index, pos):	
		logi.logiWrite(address+((index * 2) + 1), ((pos & 0x00FF), ((pos >> 8) & 0x00FF)));
	
def setServoFailSafeAngle(address, index, angle, max_pulse=255.0, min_angle=-45.0, max_angle=45.0):
	quanta = max_pulse/(max_angle-min_angle)
        pulse = (max_pulse/2) + (quanta * angle)
        pulse = max(min(int(round(pulse)), 255), 0)
	logi.logiWrite(address+((index * 2) + 1), ((pulse & 0x00FF), ((pulse >> 8) & 0x00FF)));

def setServoAngle(address, index, angle, max_pulse=255.0, min_angle=-45.0, max_angle=45.0):
	quanta = max_pulse/(max_angle-min_angle)
        pulse = (max_pulse/2) + (quanta * angle)
	pulse = max(min(int(round(pulse)), 255), 0)
	setServoPulse(address, index, int(round(pulse)))

def setPWMDivider(address, divider):	
		logi.logiWrite(address, ((divider & 0x00FF), ((divider >> 8) & 0x00FF)));

def setPWMPeriod(address, period):	
		logi.logiWrite(address+1, ((period & 0x00FF), ((period >> 8) & 0x00FF)));

def setPWMPulse(address, chan, pulse):	
		logi.logiWrite(((address+2)+chan), ((pulse & 0x00FF), ((pulse >> 8) & 0x00FF)));

def setGPIODir(address, dir_mask):	
		logi.logiWrite(address+1, ((dir_mask & 0x00FF), ((dir_mask >> 8) & 0x00FF)));

def setGPIOVal(address, val):	
		logi.logiWrite(address, ((val & 0x00FF), ((val >> 8) & 0x00FF)));

def getGPIOVal(address):	
		read_val = logi.logiRead(address, 2);
		return read_val[0]+(read_val[1] << 8)

def readRegister(address, reg):	
		read_val = logi.logiRead(address+reg, 2);
		return read_val[0]+(read_val[1] << 8)

def writeRegister(address, reg, val):	
		read_val = logi.logiWrite(address+reg, ((val & 0x00FF), ((val >> 8) & 0x00FF)))


def enableWatchdog(address):	
		logi.logiWrite(address, (0x01, 0x00) );

def disableWatchdog(address):	
		logi.logiWrite(address, (0x00, 0x00) );

def resetWatchdog(address):	
		logi.logiWrite(address, (0x01, 0x00) );
		
def setSegBCD(address, val):
	decode_sseg = [0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0$
	buf = ()
	for i in val :
		buf = buf + (decode_sseg[i],)
	while len(buf) < 6 :
		buf = buf + (0x00,)
	logi.logiWrite(address, buf)

		


