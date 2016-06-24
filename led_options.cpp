#include "mbed.h"

void flashLED(DigitalOut myled)
{
    myled = 0;
    wait(0.2);
    myled = 1;
    wait(0.2);
    myled = 0;
    wait(0.2);
    myled = 1;
    wait(0.5);
}