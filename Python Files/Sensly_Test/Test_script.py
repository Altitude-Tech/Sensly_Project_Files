from Sensors_v1_6 import *
import smbus as I2C
from time import sleep
import Adafruit_BME280
import RPi.GPIO as GPIO

R0 = [3120.5010, 1258.8822, 2786.3375]
RSAir = [9.5,27,3.62] 
MQ2 = Sensor('MQ2',R0[0],RSAir[0])
MQ7 = Sensor('MQ7',R0[1],RSAir[1])
MQ135 = Sensor('MQ135',R0[2],RSAir[2])
PM = Sensor('PM',0,0)

MQ2_INDEX = 0x01
MQ7_INDEX = 0x02
MQ135_INDEX = 0x03
PM_INDEX = 0x04

LED = Gas('MQ2_H2', 0.3222, -0.4750, -2.0588, 3.017, 100, [255,0,255]) # name, rs_r0_ratio max value, rs_r0_ratio max value, gas conversion formula gradient, gas conversion formula intercept, threshold, LED Colour
i2c = I2C.SMBus(1)  
bME280_Sensor = Adafruit_BME280.BME280(t_mode=Adafruit_BME280.BME280_OSAMPLE_8, h_mode=Adafruit_BME280.BME280_OSAMPLE_8, address=0x76) # Sometimes Crash with a connection timeout
sensor = bme680.BME680()

# Reseting the HAT
def Reset():
    GPIO.setmode(GPIO.BCM) ## Use BCM numbering
    GPIO.setup(23, GPIO.OUT)
    # Reset Sensly HAT
    GPIO.output(23, False) ## Set GPIO Pin 23 to low
    time.sleep(0.8)
    GPIO.output(23, True) ## Set GPIO Pin 23 to low
    time.sleep(0.5)
    # Clean up the GPIO pins 
    # GPIO.cleanup() # unusefull

temperature = bME280_Sensor.read_temperature()

humidity = bME280_Sensor.read_humidity()

Reset()

sleep(1)

MQ2raw = MQ2.Get_rawdata(MQ2_INDEX)
sleep(1)
MQ7raw = MQ7.Get_rawdata(MQ7_INDEX)
sleep(1)
MQ135raw = MQ135.Get_rawdata(MQ135_INDEX)
sleep(1)
PMraw = PM.get_optical_dust_sensor_rawdata()

sleep(2)
print "\n \n"
print "********************** Output *****************************\n"

print "Temperature: ", temperature 
print "\n"
print "Humidity: ", humidity 
print "\n"
print "MQ2 adc: ",MQ2raw 
print "\n"
print "MQ7 adc: ",MQ7raw 
print "\n"
print "MQ135 adc: ",MQ135raw 
print "\n"
print "PM adc: ", PMraw 

GPIO.cleanup()
