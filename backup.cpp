/*
//DAC Control NINO32

#include "mbed.h"

//#include "USBSerial.h"

//Serial pc(USBTX, USBRX);
Serial pc(USBTX, USBRX); // tx, rx

//SPI temp_port(p5, p6, p7); //mosi, miso, sclk

//SPI dac_port(p11, p12, p13); //mosi, miso, sclk

SPI dac_port(PTD2, PTD3, PTD1); //mosi, miso, sclk
DigitalOut sync(PTD0);
DigitalOut ldac(PTD7);
DigitalOut myled(LED1);

//Timer timer_slow;
//Timer timer_fast;
//char com_byte;

unsigned short dac_data[9];
float volt[8];


//char config_byte;
//char dummy_byte;

unsigned short dummy_2byte;

// dac_data

int i, j;

int main()

{
    pc.printf("Starting configuration.\n");
    myled=0;
    wait(0.2);
    myled=1;
    wait(0.2);
    myled=0;
    wait(0.2);
    myled=1;
    wait(0.2);
    

// voltage exmaple fff - 2.5v 800 - 1.25vs
    dac_data[0] = 0xE000; // DAC reset
    dac_data[1] = 0x0706; //channel 0 TH +1 U1
    dac_data[2] = 0x1666; //channel 1 TH -1 U1
    dac_data[3] = 0x2706; //channel 2 THB +1 U1
    dac_data[4] = 0x3666; //channel 3 THB -1 U1
    dac_data[5] = 0x468e; //channel 4 TH +2  U2 -----
    dac_data[6] = 0x5666; //channel 5 TH -2  U2 ------
    dac_data[7] = 0x6706; //channel 6 THB +2 U2
    dac_data[8] = 0x7666; //channel 7 THB -12 U2
 
    pc.printf("DEBUG 1.\n"); 
    myled = 0;

    for (i = 0; i < 9; i++) {
        pc.printf("DEBUG %d_0.\n", i+2); 
        dac_port.format(8,1); //SPI setup for dac chips 8 bit data, mode 1
        pc.printf("DEBUG %d_a.\n", i+2); 
        dac_port.frequency(10000);
        sync = 0;
        ldac = 1;
        wait (1);

        pc.printf("DEBUG %d_b.\n", i+2);
        sync = 1;
        ldac = 1;
        wait (0.0001);


        sync = 0;
        ldac = 1;
        dac_port.write(dac_data[i]);

        //  dac_port.write(i*4096 + vot[i]/2.50*4095);

//   dac_port.write(30720);
        sync = 0;
        ldac = 0;

        pc.printf("DEBUG %d_c.\n", i+2);
        wait (0.0001);
        sync = 0;
        ldac = 1;

        wait (1);

    }

 
    pc.printf("DEBUG END.\n"); 
    myled = 0;
    
    while(1) {
        myled = 0;
        wait(0.2);
        myled = 1;
        wait(0.2);
        myled = 0;
        wait(0.2);
        myled = 1;
        wait(0.5);
    }
 

 

}
*/