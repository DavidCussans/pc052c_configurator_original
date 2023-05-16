#include "mbed.h"


int sendConfiguration(SPI &dac_port, DigitalOut &sync, bool scan_mode);

int sendTransfer(unsigned short data,  SPI &dac_port, DigitalOut &sync);

int mySPISend( unsigned short data, SPI &dac_port, DigitalOut &sync);

int powerMode(SPI &dac_port, DigitalOut &sync, int mode);

int setPort(SPI &dac_port, DigitalOut &sync, int myPort, int newState);

int mySPIRead(unsigned short data, SPI &dac_port, DigitalOut &sync, bool scan_mode);