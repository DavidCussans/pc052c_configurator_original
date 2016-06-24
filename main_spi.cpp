//pc052a_configurator
//P. Baesso  - paolo.baesso@bristol.ac.uk
//This firmware can be installed on a KL25Z board and provides
//a serial interface to configure the pc052a board.
//The communication between KL25Z and pc052a uses the SPI interface.

#include "mbed.h"
#include <led_options.h>
#include <spi_functions.h>

// DEFINE SERIAL INTERFACE AND PINS USED BY THE SPI PROTOCOL
//Serial pc(USccBTX, USBRX);
Serial pc(USBTX, USBRX); // tx, rx
DigitalOut myled(LED1);
SPI dac_port(PTD2, PTD3, PTD1); //mosi, miso, sclk
DigitalOut sync(PTD5, 1);// slave select. Initialize to high
//DigitalOut ldac(PTD7);

unsigned short myData; 
int i, j;

int main()
{
    pc.printf("\nStarting interface.\n");
    pc.printf("Press 'h' for a list of commands.\n");
    flashLED( myled);
    dac_port.format(8,0);   //SPI setup for dac chips 8 bit data, mode 1 (falling edge), mode 0 (rising edge, active clock is 1). Max7301A requires rising edge.
    dac_port.frequency(100000);

    
    //Initialize variable used to select cases
    char cUser;
    while(cUser != 'e')
    {
        cUser = pc.getc();
        switch(cUser)
        {
             case 'b' :
                pc.printf("blinking...\n");
                flashLED( myled);
                break;
            case 'c' :
                pc.printf("Configuring...\n" );
                sendConfiguration(pc,  dac_port, sync);
                wait(1);
                printf("Configuring done!\n");
                break;
            case 'e' :
                pc.printf("End of program. Press the reset button to start again.\n" );
                break;
            case 'h' :
                pc.printf("Press:\n'b' to blink.\n'c' to configure.\n't' to transfer.\n'p' to power up.\n's' to shutdown the chip.\n'e' to exit.\n" );
                break;
            case 'i' :
                myData= 0x80C0;
                pc.printf("WRITING (0x%04x).\n", myData);
                mySPISend( myData, dac_port, sync);
                break;
            case 'p' :
                myData= 0x0401;
                pc.printf("POWER UP. WRITING (0x%04x).\n", myData);
                mySPISend( myData, dac_port, sync);
                break;
            case 's' :
                myData= 0x0400;
                pc.printf("SHUT DOWN. WRITING (0x%04x).\n", myData);
                mySPISend( myData, dac_port, sync);
                break;
            case 't' :
                pc.printf("Configuring... (with transfer)\n");
                myData= 0x55C0;
                sendTransfer(myData, dac_port, sync);
                break;
            default :
                printf("Invalid command. Press 'h' for a list of commands.\n" );
        }
    }
 

}