#include "mbed.h"


int sendConfiguration(Serial &serialOut,  SPI &dac_port, DigitalOut &sync);

int sendTransfer(unsigned short data,  SPI &dac_port, DigitalOut &sync);

int mySPISend( unsigned short data, SPI &dac_port, DigitalOut &sync);