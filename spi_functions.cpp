#include "mbed.h"
#include <spi_functions.h>

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
    
    //SET ALL OUTPUTS TO 1
    dac_data[7] = 0x44FF; // P4 - P11
    dac_data[8] = 0x4CFF; // P12 - P19
    dac_data[9] = 0x54FF; // P20 - P27
    dac_data[10]= 0x5CFF; // P28 - P31
    
    //EXIT SHUTDOWN MODE
    dac_data[11]= 0x0401;
    /*
    dac_data[0] = 0x0B5A; //
    dac_data[1] = 0x0C55; //
    dac_data[2] = 0x0D55; // 
    dac_data[3] = 0x4E00; //
    dac_data[4] = 0x5600; //
    dac_data[5] = 0x0401; //
    */
    for (int i = 0; i < nWords; i++)
    {
        serialOut.printf("WRITING %d (0x%04x).\n", dac_data[i], dac_data[i]);
        mySPISend(dac_data[i], dac_port, sync);
    }
    return 0;
}

int sendTransfer(unsigned short data,  SPI &dac_port, DigitalOut &sync)
{
    
    //unsigned char nibble_data[2];
    //nibble_data[0]= data >>8;
    //nibble_data[1]= data;
    
    //unsigned char rx_buffer[2];
    
    //sync = 1;
    //wait (0.5);
    
    //sync = 0;
    
    //dac_port.transfer(nibble_data , 2, rx_buffer, 2, *functionpointer);
    //dac_port.write(data >> 8);// We have a 8 bit interface but need to write 16 bits. Two write in sequence (or use "transfer" instead)
    //dac_port.write(data);

    //sync = 1;

    //wait (0.5);
    
    
    return 0;    
}


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

