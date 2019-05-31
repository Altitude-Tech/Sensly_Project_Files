#n Module that contains the functions necessary for the operation
# of the Sensly HAT

# Import common modules needed for script
import time
from math import log10
import logging
import sys

# Import installed modules needed for script
import smbus as I2C

# Define the i2c address for the Sensly HAT
I2C_HAT_ADDRESS = 0x0a
POT_ADDRESS = 0x2e

# LED Constants
# Array containing RGB values for LED [RED Brightness, Green Brightness, Blue Brightness]
Green = [0x00,0x00,0xFF] 
Orange = [0xFF,0x09,0x00]
Off = [0x00,0x00,0x00]

# i2c cmd to set LED value
LEDcmd = 0x07 

# MQ Constants for interpreting raw sensor data
MaxADC = 4095
RLOAD = 10000


#*************************************************************************************************
# Class sensor MQ2 MQ7 MQ135
#*************************************************************************************************
class Sensor:

    # PM Constants for interpreting raw PM data seems there is some doc on https://www.sparkfun.com/datasheets/Sensors/gp2y1010au_e.pdf
    NODUSTVOLTAGE = 500
    COVRATIO = 0.2
    OPTICAL_DUST_SENSOR_CMD = 0x04
    OPTICAL_DUST_SENSOR_SAMPLING_TIME = 0.4 #28ms in doc
    RS = None # Stands for resistance of sensor

    def __init__(self, name, R0, RSAIR):
        logging.basicConfig(stream=sys.stdout, level=logging.DEBUG, format='%(asctime)s.%(msecs)03d %(levelname)s %(module)s - %(funcName)s: %(message)s', datefmt="%Y-%m-%d %H:%M:%S")
        self.name = name
        i2c = I2C.SMBus(1)  
        self._device = i2c
        self.R0 = R0
        self.RLOAD = RLOAD
        self.RSAIR = RSAIR
        self.MaxADC = MaxADC
        
    # Function to get raw data for the sensors from the Sensly HAT via the i2c peripheral
    def Get_rawdata(self, cmd):
        data = []
        logging.debug("Writting byte {} with index {}".format(I2C_HAT_ADDRESS, cmd))
        self._device.write_byte(I2C_HAT_ADDRESS, cmd)
        time.sleep(0.1)
        bt = self._device.read_byte(I2C_HAT_ADDRESS)
        logging.debug("Read byte is '{}'".format(bt))
        data.append(bt)
        time.sleep(0.1)
        bt = self._device.read_byte(I2C_HAT_ADDRESS)
        logging.debug("Read byte is '{}'".format(bt))
        data.append(bt)

        self.Raw = data[0] 
        self.Raw = (self.Raw<<8) | data[1]
        logging.debug("Raw Data returned value is %s" % self.Raw)
        return self.Raw
    
    
    def get_optical_dust_sensor_rawdata(self):
        """# Function to get raw data for the sensors from the Sensly HAT via the i2c peripheral"""
        data = []
        logging.debug("Writting byte {} with index {}".format(I2C_HAT_ADDRESS, self.OPTICAL_DUST_SENSOR_CMD))
        self._device.write_byte(I2C_HAT_ADDRESS, self.OPTICAL_DUST_SENSOR_CMD)
        time.sleep(self.OPTICAL_DUST_SENSOR_SAMPLING_TIME)
        byteRead = self._device.read_byte(I2C_HAT_ADDRESS)
        logging.debug("Byte read from dust sensor:" + str(byteRead))
        data.append(byteRead)
        time.sleep(self.OPTICAL_DUST_SENSOR_SAMPLING_TIME)
        byteRead = self._device.read_byte(I2C_HAT_ADDRESS)
        logging.debug("Byte read from dust sensor:" + str(byteRead))
        data.append(byteRead)
        logging.debug("Optical dust sensor read raw values are:" + str(data))

        self.Raw = data[0] 
        self.Raw = (self.Raw<<8) | data[1]
        logging.debug("Optical dust sensor returned value is {}".format(self.Raw))
        return self.Raw


    """Setting self.RS value
     Function to convert the raw data to a resistance value 
    """
    def Get_RS(self,cmd):
        raw_data = self.Get_rawdata(cmd)
        self.RS = ((float(self.MaxADC)/float(raw_data)-1)*self.RLOAD)
        logging.debug("Raw data %s corrected to %s, should be a resistance value" % (raw_data, self.RS))
        return self.RS
    
    # Function to calculate the RS(Sensor Resistance)/R0(Base Resistance) ratio    
    def Get_RSR0Ratio(self,cmd):
        self.rsro = float(self.Get_RS(cmd)/self.R0)
        return self.rsro
    
    # Experimental function to calibrate the MQ Sensors
    def Calibrate(self, cmd, Cal_Sample_Time):
        AvRs = 0
        for x in range(Cal_Sample_Time):
            AvRs = AvRs + self.Get_RS(cmd)
            time.sleep(1)
        AvRs = AvRs/Cal_Sample_Time
        self.RZERO = AvRs/RSAIR
        return self.RZERO
    
    
    def Get_PMVolt(self, cmd):
        """Function to calculate the voltage from raw PM data"""
        rawData = self.get_optical_dust_sensor_rawdata()
        logging.debug("Rawdata from opticalDustSensor is " + str(rawData))
        self.PMVolt = ((3300.00 / self.MaxADC) * float(rawData) * 11.00)
        return self.PMVolt
    
    
    def Get_PMDensity(self, cmd):
        """ Function to calculate the densisty of the particulate matter detected""" 
        self.Get_PMVolt(cmd)
        if (self.PMVolt >= self.NODUSTVOLTAGE):
            self.PMVolt -= self.NODUSTVOLTAGE
            self.PMDensity = self.PMVolt * self.COVRATIO
        else:
            self.PMDensity = 0            
        return self.PMDensity
    
    # Function to correct the RS/R0 ratio based on temperature and relative humidity
    def Corrected_RS_RO(self, cmd, temperature, humidity, Const_33 = [], Const_85 = []):
        rsro_ambtemp_33RH = (Const_33[0]*pow(temperature,3)) + (Const_33[1]*pow(temperature,2)) + (Const_33[2]*temperature) + Const_33[3]
        rsro_ambtemp_85RH = (Const_85[0]*pow(temperature,3)) + (Const_85[1]*pow(temperature,2)) + (Const_85[2]*temperature) + Const_85[3]
        rsro_ambtemp_65RH = ((65.0-33.0)/(85.0-65.0)*(rsro_ambtemp_85RH-rsro_ambtemp_33RH)+rsro_ambtemp_33RH)*1.102
        if humidity < 65:
            rsro_ambtemp_ambRH = (humidity-33)/(65-33)*(rsro_ambtemp_65RH-rsro_ambtemp_33RH)+rsro_ambtemp_33RH
        else:
            rsro_ambtemp_ambRH = (humidity-65)/(85-65)*(rsro_ambtemp_85RH-rsro_ambtemp_65RH)+rsro_ambtemp_65RH
        #calculate correction factor
        refrsro_at_20C65RH = 1.00
        rsroCorrPct = 1 + (refrsro_at_20C65RH - rsro_ambtemp_ambRH)/ refrsro_at_20C65RH
        correctedrsro = rsroCorrPct * (self.Get_RSR0Ratio(cmd))
        return correctedrsro
    
    # Function to correct the RS/R0 ratio based on temperature and relative humidity for the MQ2
    def Corrected_RS_RO_MQ2(self, cmd, temperature, humidity, Const_30 = [], Const_60 = [], Const_85 = []):
        rsro_ambtemp_30RH = (Const_30[0]*pow(temperature,3)) + (Const_30[1]*pow(temperature,2)) + (Const_30[2]*temperature) + Const_30[3]
        rsro_ambtemp_60RH = (Const_60[0]*pow(temperature,3)) + (Const_60[1]*pow(temperature,2)) + (Const_60[2]*temperature) + Const_60[3]
        rsro_ambtemp_85RH = (Const_85[0]*pow(temperature,3)) + (Const_85[1]*pow(temperature,2)) + (Const_85[2]*temperature) + Const_85[3]
        
        if humidity < 60:
            rsro_ambtemp_ambRH = (humidity-30)/(60-30)*(rsro_ambtemp_60RH-rsro_ambtemp_30RH)+rsro_ambtemp_30RH
        else:
            rsro_ambtemp_ambRH = (humidity-60)/(85-60)*(rsro_ambtemp_85RH-rsro_ambtemp_60RH)+rsro_ambtemp_60RH
        # Calculate correction factor
        refrsro_at_20C60RH = 1.00
        rsroCorrPct = 1 + (refrsro_at_20C60RH - rsro_ambtemp_ambRH)/ refrsro_at_20C60RH
        correctedrsro = rsroCorrPct * (self.Get_RSR0Ratio(cmd))
        return correctedrsro
    
#*************************************************************************************************
# Class Gaz
#*************************************************************************************************
class Gas:
    
    def __init__(self,name,rsromax,rsromin,gradient,intercept, threshold, LED = []):
        self.name = name
        logging.basicConfig(stream=sys.stdout, level=logging.DEBUG, format='%(asctime)s.%(msecs)03d %(levelname)s %(module)s - %(funcName)s: %(message)s', datefmt="%Y-%m-%d %H:%M:%S")
        i2c = I2C.SMBus(1)  
        self._device = i2c
        self.min = rsromin
        self.max = rsromax
        self.gradient = gradient
        self.intercept = intercept
        self.threshold = threshold
        self.LED = LED
        LEDcmd = 0x07

    
    def Get_PPM(self, rs_ro):
        """ Function to calculate the PPM of the specific gas"""
        self.PPM = pow(10,((self.gradient*(log10(rs_ro)))+self.intercept))
        logging.debug("Getting PPM gave %s" % self.PPM)
        return self.PPM
    
    # Function to set the LED Color, used for setting alarms points
    def Set_LED(self, LEDColour):  # LEDColour = Red , Green, Blue Brightness values from 0 - 255 in an array   
        self._device.write_byte(I2C_HAT_ADDRESS,LEDcmd)
        logging.debug("Writting byte {} with index {} for Setting LED Color".format(I2C_HAT_ADDRESS, LEDcmd))
        time.sleep(0.1)
        for x in range(3):
            logging.debug("Writting byte {} with index {}".format(I2C_HAT_ADDRESS, LEDColour[x]))
            self._device.write_byte(I2C_HAT_ADDRESS, LEDColour[x])
            time.sleep(0.1)
            
    # Function to check the PPM value against the predefined threshold         
    def Chk_threshold(self):
        if self.PPM < self.threshold:
            self.Set_LED(Green)
        elif self.PPM == self.threshold:
            self.Set_LED(Orange)
        elif self.PPM > self.threshold:
            self.Set_LED(self.LED)

#*************************************************************************************************
# Class Digital Potentiometer
#*******************************************************************************
class Pot:

    def __init__(self):
        
        self.Max_Resistance = 50
        self.Max_Val = 128
        i2c = I2C.SMBus(1)  
        self._device = i2c

    '''
        Function to calculate ADC value from Resistance value 
    '''
    def Calc_ADC(self,Resistance):
        Value_to_Send = (self.Max_Val * Resistance) / self.Max_Resistance
        return Value_to_Send
    
    '''
        Function to calculate Resistance value from ADC value 
    '''
    def Calc_Resistance(self,ADC_Val):
        Resistance = (ADC_Val * self.Max_Resistance) / self.Max_Val
        return Resistance
    
    ''' Function to write resistance value to digital pot 
        Params:
        Wiper - From left to right
                0 represents Rload for Sensor 1,
                1 represents Rload for Sensor 2
                2 represents Rload for Sensor 3
        Resistance - This is the resistance value you want 
    '''
    def Write_to_Pot(self,Sensor, Resistance):
         
        if Sensor == 0:
            Command_Byte = 0x30
        if Sensor == 1:
            Command_Byte = 0x90
        if Sensor == 2:
            Command_Byte = 0x80
        

        # Calculate value to be sent to get requested resistance
        Data_Byte = self.Calc_ADC(Resistance)

        self._device.write_i2c_block_data(POT_ADDRESS,Command_Byte,[Data_Byte])

        time.sleep(0.5)

    '''
        Function to read the Resistance value from memory
        Params:
        Wiper - From left to right
                0 represents Rload for Sensor 1,
                1 represents Rload for Sensor 2
                2 represents Rload for Sensor 3
    '''
    def Read_from_Pot(self, Sensor):

        data = []
        
        if Sensor == 0:
            Command_Byte = 0x3C
        if Sensor == 1:
            Command_Byte = 0x9C
        if Sensor == 2:
            Command_Byte = 0x8C

        data = self._device.read_i2c_block_data(POT_ADDRESS,Command_Byte)

        Raw = data[0] 
        Raw = (Raw<<8) | data[1]
        Resistance = self.Calc_Resistance(Raw)
        return Resistance

    def Write_all_Sensors_Resistance(self, Resistance):
        
        for x in range(3):
            self.Write_to_Pot(x, Resistance)
                
    def Read_all_Sensors_Resistance(self):
        self.Resistance = [] 
        for x in range(3):
            self.Resistance.append(self.Read_from_Pot(x))
        return self.Resistance   
