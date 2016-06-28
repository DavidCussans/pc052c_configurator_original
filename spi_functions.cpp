#include "mbed.h"
#include <spi_functions.h>

////////////////////////////////////////////////////////
int sendConfiguration(Serial &serialOut,  SPI &dac_port, DigitalOut &sync)
{
    int nWords= 12;
    unsigned short dac_data[nWords];
    
    //CONFIGURE ALL PORTS AS OUTPUTS
    dac_data[0] = 0x0955; //
    dac_data[1] = 0x0A55; //
    dac_data[2] = 0x0B55; // 
    dac_data[3] = 0x0C55; //
    dac_data[4] = 0x0D55; //
    dac_data[5] = 0x0E55; //
    dac_data[6] = 0x0F55; //
    
    //SET ALL OUTPUTS TO 0
    dac_data[7] = 0x4400; // P4 - P11
    dac_data[8] = 0x4C00; // P12 - P19
    dac_data[9] = 0x5400; // P20 - P27
    dac_data[10]= 0x5C00; // P28 - P31
    
    //EXIT SHUTDOWN MODE
    dac_data[11]= 0x0401;
    for (int i = 0; i < nWords; i++)
    {
        serialOut.printf("WRITING %d (0x%04x).\n", dac_data[i], dac_data[i]);
        mySPISend(dac_data[i], dac_port, sync);
    }
    return 0;
}

////////////////////////////////////////////////////////
int sendTransfer(unsigned short data,  SPI &dac_port, DigitalOut &sync)
{
   
    return 0;    
}

////////////////////////////////////////////////////////
int mySPISend( unsigned short data, SPI &dac_port, DigitalOut &sync)
// This is the basic function used to send the signals.
{
    //sync = 1;
    wait (0.5);

    sync = 0;
    
    dac_port.write(data >> 8);// We have a 8 bit interface but need to write 16 bits. Two write in sequence (or use "transfer" instead)
    dac_port.write(data);
    
    wait(0.000005); //
    sync = 1;

    wait (0.5);
    return 0;    
}

////////////////////////////////////////////////////////
int mySPIRead(Serial &serialOut,  unsigned short data, SPI &dac_port, DigitalOut &sync)
// This is the basic function used to send the signals.
{
    unsigned short nopdata= 0x0000;
    unsigned short return1, return2;
    wait (0.5);

    sync = 0;
    
    dac_port.write(data >> 8);// We have a 8 bit interface but need to write 16 bits. Two write in sequence (or use "transfer" instead)
    dac_port.write(data);
    
    wait(0.000005); //
    sync = 1;
    
    wait (0.2);
    sync = 0;
    
    return1= dac_port.write(nopdata >> 8);// No operation. Used to flush the 16 bits.
    return2= dac_port.write(nopdata);
    
    wait(0.000005); //
    sync = 1;
    wait (0.5);
    serialOut.printf("Returned data = %d (0x%04x), %d (0x%04x).\n", return1, return1, return2, return2);
    return 0;    
}

////////////////////////////////////////////////////////
int powerMode(SPI &dac_port, DigitalOut &sync, int mode)
{
    int nWords= 2;
    unsigned short dac_data[nWords];
    
    
    dac_data[0]= 0x0400;//ENTER SHUTDOWN MODE
    dac_data[1]= 0x0401;//EXIT SHUTDOWN MODE
    
    if ((mode==0) || (mode==1))
    {
        mySPISend(dac_data[mode], dac_port, sync);
    }

    return 0;
}

////////////////////////////////////////////////////////
int setPort(SPI &dac_port, DigitalOut &sync, int myPort, int newState)
{
    int nWords= 1;
    unsigned short dac_data[nWords];
    
    dac_data[0]= (myPort + 0x0020)*0x0100 + newState;
    mySPISend(dac_data[0], dac_port, sync);
    /*
    dac_data[0]= 0x0400;//ENTER SHUTDOWN MODE
    dac_data[1]= 0x0401;//EXIT SHUTDOWN MODE
    
    if ((mode==0) || (mode==1))
    {
        mySPISend(dac_data[mode], dac_port, sync);
    }*/
    return 0;
}