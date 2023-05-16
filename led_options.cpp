#include "mbed.h"

void flashLED(DigitalOut myled)
{
    myled = 0;
    wait_us(200000);
    myled = 1;
    wait_us(200000);
    myled = 0;
    wait_us(200000);
    myled = 1;
    wait_us(200000);
    myled = 0;
    wait_us(200000);
    myled = 1;
    wait_us(200000);
}