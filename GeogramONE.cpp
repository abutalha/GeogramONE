#include "GeogramONE.h"

template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const char* p = (const char*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
        EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
    char* p = (char*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
		*p++ = EEPROM.read(ee++);
    return i;
}


GeogramONE::GeogramONE()
{
}


void GeogramONE::goToSleep()
{
	delay(200);  //need delay because GSM module is shutting down. 1000 works
	uint8_t pcicrReg = PCICR; //backup the current Pin Change Interrupt register
	PCICR = 0; // disable all pin change interrupts. Need to do this because of NewSoftSerial

	pinMode(9,INPUT);  //shut off NewSoftSerial Tx pin 
	digitalWrite(9,LOW); //set to high impedance
	digitalWrite(8,LOW); // set NewSoftSerial Rx pin to high impedance
	set_sleep_mode (SLEEP_MODE_PWR_DOWN);
	sleep_enable();	
	MCUCR = _BV (BODS) | _BV (BODSE);
	MCUCR = _BV (BODS);
	sleep_cpu ();
  
/*********ATMEGA is sleeping at this point***************/  
	sleep_disable();
	pinMode(9,OUTPUT); //restore NewSoftSerial settings
	digitalWrite(9,HIGH);
	digitalWrite(8,HIGH);
	PCICR = pcicrReg; //restore Pin Change Interrupt register
}

void GeogramONE::init()
{
	pinMode(PG_INT ,INPUT);
	digitalWrite(PG_INT ,LOW);
	configureIO(4,IOSTATE0); //D4
	configureIO(10,IOSTATE1);//D10
	configureIO(15,IOSTATE2);//A1
	configureIO(16,IOSTATE3);//A2
	configureIO(17,IOSTATE4);//A3
	configureIO(20,IOSTATE5);//A6
}

void GeogramONE::configureIO(uint8_t pinNumber, uint8_t eepromAddress)
{
	uint8_t ioConfig = 0;
	ioConfig = EEPROM.read(eepromAddress);
	if(ioConfig == 0){pinMode(pinNumber,INPUT);digitalWrite(pinNumber,LOW);}
	if((ioConfig == 1) || (ioConfig == 5) || (ioConfig == 6)){pinMode(pinNumber,INPUT);digitalWrite(pinNumber,HIGH);}
	if(ioConfig == 2){pinMode(pinNumber,OUTPUT);digitalWrite(pinNumber,LOW);}
	if(ioConfig == 3){pinMode(pinNumber,OUTPUT);digitalWrite(pinNumber,HIGH);}
	if(ioConfig == 4){analogReference(DEFAULT);analogRead(pinNumber - 14);}
}

void GeogramONE::configureFence(uint8_t fenceNumber, geoFence *fence)
{
	uint8_t offset = 0;
	if(fenceNumber == 2)
		offset += 14;
	if(fenceNumber == 3)
		offset += 28;
	EEPROM_readAnything((INOUT1 + offset), fence->inOut);
	EEPROM_readAnything((RADIUS1 + offset), fence->radius);
	EEPROM_readAnything((LATITUDE1 + offset), fence->latitude);
	EEPROM_readAnything((LONGITUDE1 + offset), fence->longitude);
	return ;
}

void GeogramONE::configureBreachParameters(uint8_t *breachS, uint8_t *breachR)
{
	*breachS = EEPROM.read(BREACHSPEED);
	*breachR = EEPROM.read(BREACHREPS);
}


void GeogramONE::getFenceActive(uint8_t fenceNumber, uint8_t *fenceVar)
{
	uint8_t offset = 0;
	if(fenceNumber == 2)
		offset += 14;
	if(fenceNumber == 3)
		offset += 28;
	EEPROM_readAnything(ACTIVE1 + offset,*fenceVar);
	return ;
}

void GeogramONE::configureSleepTime(uint32_t *sleepTimeOn, uint32_t *sleepTimeOff, uint8_t *sleepTimeConfig)
{
	EEPROM_readAnything(SLEEPTIMEON,*sleepTimeOn);
	EEPROM_readAnything(SLEEPTIMEOFF,*sleepTimeOff);
	EEPROM_readAnything(SLEEPTIMECONFIG,*sleepTimeConfig);
	return;
}

void GeogramONE::configureSpeed(uint8_t *cmd3, uint8_t *speedH, uint16_t *speedL)
{
	EEPROM_readAnything(SPEEDLIMIT,*speedL);
	*speedH = EEPROM.read(SPEEDHYST);
	if(*speedL)
		*cmd3 = 0x02;
	else
		*cmd3 = 0x00;
}