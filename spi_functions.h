#include "mbed.h"


int sendConfiguration(Serial &serialOut,  SPI &dac_port, DigitalOut &sync);

int sendTransfer(unsigned short data,  SPI &dac_port, DigitalOut &sync);

int mySPISend( unsigned short data, SPI &dac_port, DigitalOut &sync);

int powerMode(SPI &dac_port, DigitalOut &sync, int mode);

int setPort(SPI &dac_port, DigitalOut &sync, int myPort, int newState);

int mySPIRead(Serial &serialOut,  unsigned short data, SPI &dac_port, DigitalOut &sync);