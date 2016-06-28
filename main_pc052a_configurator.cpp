//pc052a_configurator
//P. Baesso  - paolo.baesso@bristol.ac.uk
//This firmware can be installed on a KL25Z board and provides
//a serial interface to configure the pc052a board.
//The communication between KL25Z and pc052a uses the SPI interface.

#include "mbed.h"
#include <led_options.h>
#include <spi_functions.h>
#include <vector>
#include <string> 

std::vector<std::string> tokenize(const std::string& str, const std::string& delim);
void doCommands(std::vector<std::string>& tokens);
using namespace std;

// DEFINE SERIAL INTERFACE AND PINS USED BY THE SPI PROTOCOL
//Serial pc(USccBTX, USBRX);
Serial pc(USBTX, USBRX); // tx, rx
DigitalOut myled(LED1);
SPI dac_port(PTD2, PTD3, PTD1); //mosi, miso, sclk
DigitalOut sync(PTD5, 1);// slave select. Initialize to high
//DigitalOut ldac(PTD7);

unsigned short myData; 
int i, j;
int runFlag=1;
string myCommand;

int main()
{
    pc.printf("\nStarting interface.\n");
    pc.printf("Enter commands and press TAB to send.\n");
    pc.printf("Type 'help'+TAB for a list of commands.\n");
    flashLED( myled);
    dac_port.format(8,0);   //SPI setup for dac chips 8 bit data, mode 1 (falling edge), mode 0 (rising edge, active clock is 1). Max7301A requires rising edge.
    dac_port.frequency(100000);

    
    //Initialize variable used to select cases
    char cUser;
    while(runFlag)
    {
        if(pc.readable())
        {
            cUser = pc.getc();
            wait(0.02f); //
            if (cUser== '\t')// \n"
            {
                pc.printf("End of line detected. Parsing command.\n", myCommand.c_str());
                
                //PARSE COMMANDS
                string delim= " ";
                vector<string> myTokens = tokenize(myCommand, delim);
                
                pc.printf("%d TOKENS FOUND.\n", myTokens.size());
                pc.printf("TOKENS= ");
                for (int iTk=0; iTk <myTokens.size(); iTk++)
                {
                    pc.printf("%s - ", myTokens.at(iTk).c_str());
                }
                pc.printf("\n");
                doCommands(myTokens);

                   
                //RESET WORD
                myCommand= "";
            }
            else
            {
                myCommand= myCommand+ cUser;
                pc.printf("So far I got: %s\r", myCommand.c_str());
            }
        }
        
        
    }
    pc.printf("Interface stopped. Press the reset button to start again.\n");
    wait(1.0f);

}


//////////////////////////////////////////
std::vector<std::string> tokenize(const std::string& str, const std::string& delim)
{
  std::vector<std::string> tokens;

  size_t pos = 0;
  size_t loc = str.find_first_of(delim, pos);

  if (loc == std::string::npos)
  {
    tokens.push_back(str);
  }
  else
  {
    while (pos != std::string::npos)
    {
      tokens.push_back(str.substr(pos, loc - pos));

      pos = (loc == std::string::npos ? loc : loc + 1);
      loc = str.find_first_of(delim, pos);
    }
  }
  return tokens;
}

//////////////////////////////////////////
void doCommands(std::vector<std::string>& tokens)
{
    // Parse the command tokens    
    string firstTk= tokens.at(0);
    if(firstTk == "exit") //Exit condition
    {
        pc.printf("Stopping interface.\n");
        runFlag=0;
    }
    else if(firstTk == "help") //Help
    {
        pc.printf("Type commands and press TAB to execute them.\n Valid commands are:\n'exit'\n'help'\n'powermode'\n'configure'\n'setport'\n'send'\n");
    }
    else if(firstTk == "configure")//Send configuration
    {
        pc.printf("Configuring all ports as outputs (low).\n");
        sendConfiguration(pc,  dac_port, sync);
        pc.printf("Configuration done.\n");
    }
    else if(firstTk == "powermode")//Set power mode (shutdown or active)
    {
        if (tokens.size() ==2)
        {
            int pMode = atoi(tokens.at(1).c_str());
            if ((pMode==0) || (pMode==1))
            {
                powerMode(dac_port, sync, pMode);
                pc.printf("Power mode = %d.\n", pMode);
            }
            else
            {
                pc.printf("This function can only accept 0 or 1. You typed %d.\n");
            }
        }
        else
        {
            pc.printf("Powermode command requires 1 parameter:\ntype 'powermode 1' to power up the MAX6956\ntype 'powermode 0' to shut it down.\n");    
        }
    }
    else if(firstTk == "setport")//Set power mode (shutdown or active)
    {
        if (tokens.size() ==3)
        {
            int portN = atoi(tokens.at(1).c_str());
            int portState = atoi(tokens.at(2).c_str());
            //int toWrite=0;
            if (((portN > 3)&&(portN < 32))&&((portState==0) || (portState==1)))
            {
                pc.printf("Setting port %d to %d.\n", portN, portState);
                setPort(dac_port, sync, portN, portState);
                //toWrite=(portN + 0x0020)*0x0100 + portState;
                //pc.printf("WORD %d (0x%04x)\n", toWrite, toWrite);
            }
            else
            {
                pc.printf("Valid ports number: 7 - 31. Valid status: 0 -1. You typed port %d state %d.\n", portN, portState);
            }
            
        }
        else
        {
            pc.printf("Setport requires 2 parameters:\ntype 'setport 12 1' to set port 12 as 'high'\ntype 'setport 12 0' to set it 'low'.\nThis command assumes that the port is already configured as output.\n");    
        }
    }
    else if(firstTk == "send")
    {
        if (tokens.size() ==2)
        {
            int myWord=0;
            myWord= strtol(tokens.at(1).c_str(), NULL, 16);
            pc.printf("WORD %d (0x%04x)\n", myWord, myWord);
            mySPISend(myWord, dac_port, sync);
        }
        else
        {
            pc.printf("Send requires a hex word parameter.\ntype 'send XXXX' to send the word (x must be a hex value).\n");
        }
    }
        else if(firstTk == "read")
    {
        if (tokens.size() ==2)
        {
            int myWord=0;
            myWord= strtol(tokens.at(1).c_str(), NULL, 16);
            pc.printf("WORD %d (0x%04x)\n", myWord, myWord);
            mySPIRead(pc,  myWord, dac_port, sync);
        }
        else
        {
            pc.printf("Read requires a hex word parameter.\ntype 'send XXXX' to send the word (x must be a hex value).\n");
        }
    }
    
}