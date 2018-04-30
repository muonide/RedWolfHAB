
import piplates.DAQCplate as DAQC
import RPi.GPIO as GPIO
import time
import datetime
import sys
import signal 

global counter

counter1 = 0
counter2 = 0

def counterDET():
	global counter
	detector = DAQX.getINTflags(0)
	if detector & 0b1:
		counter1 += 1
	if detector & 0b10:
		counter2 += 1

def storecpm(value1, value2):
	f = open("log.dat", 'a')
	f.write(str(time.time()) + ",")
	f.write(str(value1) + ",")
	f.write( str(value2) + "\n")
	f.close()
 
GPIO.setup(22, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.add_event_detect(22, GPIO.FALLING, callback=counterDET)  
DAQC.enableDINint(0,0,'r')   #enable the push button interrupt
DAQC.enableDINint(0,1,'f')
DAQC.intEnable(0)     #enable global interrupts


checkpoint = time.time()

try:
           	
	if (checkpoint < time.time()):
		print "Counter is ", counter
		storecpm(counter1, counter2)
		counter1 = 0
		counter2 = 0
		checkpoint = time.time() + 60

except KeyboardInterrupt:  
    GPIO.cleanup()     
