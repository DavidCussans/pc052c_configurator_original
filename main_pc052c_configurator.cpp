//pc052c_configurator
//S.V. Addepalli & P. Baesso  - sagar.addepalli@iitb.ac.in
//This firmware can be installed on a KL25Z board and provides
//a serial interface to configure the pc052c board.
//The communication between KL25Z and pc052c uses the SPI interface.

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
SPI dac_port1(PTD2, PTD3, PTD1); //mosi, miso, sclk //SPI0
SPI dac_port2(PTD2, PTC6, PTD1);  //SPI0
SPI dac_port3(PTD2, PTC7, PTD1);    //SPI0
SPI dac_port4(PTD2, PTA16, PTD1); //SPI0
SPI dac_port5(PTD2, PTA17, PTD1); //SPI0
SPI dac_port6(PTE3, PTD7, PTE2);    //SPI1
SPI dac_port7(PTE3, PTE1, PTE2);    //SPI1
SPI dac_port8(PTE3, PTD6, PTE2);    //SPI1
DigitalOut sync1(PTD5, 1);// Board 1 slave select. Initialize to high 
DigitalOut sync2(PTD0, 1); // Board 2 slave select. Initialize to high good to go
DigitalOut sync3(PTE4, 1); // Board 3 slave select. Initialize to high GOOD TO GO
DigitalOut sync4(PTC5, 1);// Board 4 slave select. Initialize to high GOOD TO GO
DigitalOut sync5(PTB10, 1); // Board 5 slave select. Initialize to high good to go
DigitalOut sync6(PTB11, 1);// Board 6 slave select. Initialize to high 
DigitalOut sync7(PTD4, 1);// Board 7 slave select. Initialize to high good to go
DigitalOut sync8(PTC4, 1); // Board 8 slave select. Initialize to high good to go
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
    dac_port1.format(8,0);   //SPI setup for dac chips 8 bit data, mode 1 (falling edge), mode 0 (rising edge, active clock is 1). Max7301A requires rising edge.
    dac_port2.format(8,0);
    dac_port3.format(8,0);
    dac_port4.format(8,0);
    dac_port5.format(8,0);
    dac_port6.format(8,0);
    dac_port7.format(8,0);
    dac_port8.format(8,0);
    dac_port1.frequency(100000);
    dac_port2.frequency(100000);
    dac_port3.frequency(100000);
    dac_port4.frequency(100000);
    dac_port5.frequency(100000);
    dac_port6.frequency(100000);
    dac_port7.frequency(100000);
    dac_port8.frequency(100000);

    
    //Initialize variable used to select cases
    char cUser;
    while(runFlag)
    {
        if(pc.readable())
        {
            pc.printf("Entering reading condition");
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
        pc.printf("Type commands and press TAB to execute them.\n Valid commands are:\n'exit'\n'help'\n'powermode'\n'configure'\n'setport'\n'send'\n'pullallup'\n'pullalldown'\n'pulldown'\n'pullup'");
    }
    else if(firstTk == "configure")//Send configuration
    {
        pc.printf("Configuring all ports as outputs (low).\n");
        sendConfiguration(pc,  dac_port1, sync1);
        sendConfiguration(pc,  dac_port2, sync2);
        sendConfiguration(pc,  dac_port3, sync3);
        sendConfiguration(pc,  dac_port4, sync4);
        sendConfiguration(pc,  dac_port5, sync5);
        sendConfiguration(pc,  dac_port6, sync6);
        sendConfiguration(pc,  dac_port7, sync7);
        sendConfiguration(pc,  dac_port8, sync8);
        pc.printf("Configuration done.\n");
    }
    else if(firstTk == "powermode")//Set the specific board's power mode (shutdown or active)
    {

        if (tokens.size() ==3)
        {   
            int brd = atoi(tokens.at(1).c_str());
            int pMode = atoi(tokens.at(2).c_str());
            if ((pMode==0) || (pMode==1))
            {
                if(brd==1)
                {
                    powerMode(dac_port1, sync1, pMode);
                    pc.printf("Power mode for board 1 = %d.\n", pMode);
                }
                else if(brd==2)
                {
                    powerMode(dac_port2, sync2, pMode);
                    pc.printf("Power mode for board 2 = %d.\n", pMode);
                }
                else if(brd==3)
                {
                    powerMode(dac_port3, sync3, pMode);
                    pc.printf("Power mode for board 3 = %d.\n", pMode);
                }
                else if(brd==4)
                {
                    powerMode(dac_port4, sync4, pMode);
                    pc.printf("Power mode for board 4 = %d.\n", pMode);
                }
                else if(brd==5)
                {
                    powerMode(dac_port5, sync5, pMode);
                    pc.printf("Power mode for board 5 = %d.\n", pMode);
                }
                else if(brd==6)
                {
                    powerMode(dac_port6, sync6, pMode);
                    pc.printf("Power mode for board 6 = %d.\n", pMode);
                }
                else if(brd==7)
                {
                    powerMode(dac_port7, sync7, pMode);
                    pc.printf("Power mode for board 7 = %d.\n", pMode);
                }
                else if(brd==8)
                {
                    powerMode(dac_port8, sync8, pMode);
                    pc.printf("Power mode for board 8 = %d.\n", pMode);
                 }
                else
                    pc.printf("The board number has to be between 1 and 8\n");
            }
            else
            {
                pc.printf("This function can only accept 0 or 1. You typed %d.\n");
            }
        }
        else
        {
            pc.printf("Powermode command requires 2 parameters:\ntype 'powermode brdno 1' to power up the MAX6956\ntype 'powermode brdno 0' to shut it down.\n");    
        }
    }
    else if(firstTk == "setport")//Set power mode (shutdown or active)
    {
        if (tokens.size() ==4)
        {
            int brd = atoi(tokens.at(1).c_str());
            int portN = atoi(tokens.at(2).c_str());
            int portState = atoi(tokens.at(3).c_str());
            //int toWrite=0;
            if (((portN > 11)&&(portN < 32))&&((portState==0) || (portState==1)))
            {
                if(brd==1)
                {
                    pc.printf("Setting port %d to %d for board 1.\n", portN, portState);
                    setPort(dac_port1, sync1, portN, portState);
                }
                else if(brd==2)
                {
                    pc.printf("Setting port %d to %d for board 2.\n", portN, portState);
                    setPort(dac_port2, sync2, portN, portState);
                }
                else if(brd==3)
                {
                    pc.printf("Setting port %d to %d for board 3.\n", portN, portState);
                    setPort(dac_port3, sync3, portN, portState);
                }
                else if(brd==4)
                {
                    pc.printf("Setting port %d to %d for board 4.\n", portN, portState);
                    setPort(dac_port4, sync4, portN, portState);
                }
                else if(brd==5)
                {
                    pc.printf("Setting port %d to %d for board 5.\n", portN, portState);
                    setPort(dac_port5, sync5, portN, portState);
                }
                else if(brd==6)
                {
                    pc.printf("Setting port %d to %d for board 6.\n", portN, portState);
                    setPort(dac_port6, sync6, portN, portState);
                }
                else if(brd==7)
                {
                    pc.printf("Setting port %d to %d for board 7.\n", portN, portState);
                    setPort(dac_port7, sync7, portN, portState);
                }
                else if(brd==8)
                {
                    pc.printf("Setting port %d to %d for board 8.\n", portN, portState);
                    setPort(dac_port8, sync8, portN, portState);
                 }
                else
                    pc.printf("The board number has to be between 1 and 8\n");
                //toWrite=(portN + 0x0020)*0x0100 + portState;
                //pc.printf("WORD %d (0x%04x)\n", toWrite, toWrite);
            }
            else
            {
                pc.printf("Valid ports number: 12 - 31. Valid status: 0 - 1. You typed port %d state %d.\n", portN, portState);
            }
            
        }
        else
        {
            pc.printf("Setport requires 3 parameters:\ntype 'setport 5 21 1' to set port 21 on board 5 as 'high'\ntype 'setport 5 21 0' to set it 'low'.\nThis command assumes that the port is already configured as output.\n");    
        }
    }
    else if(firstTk=="pullallup")
    {
        if (tokens.size() ==1)
        {
            for(int a=12;a<=31;a++)
            {
                setPort(dac_port1, sync1, a, 1);
                setPort(dac_port2, sync2, a, 1);
                setPort(dac_port3, sync3, a, 1);
                setPort(dac_port4, sync4, a, 1);
                setPort(dac_port5, sync5, a, 1);
                setPort(dac_port6, sync6, a, 1);
                setPort(dac_port7, sync7, a, 1);
                setPort(dac_port8, sync8, a, 1);
            }
            pc.printf("Pulling all ports up in boards 1 to 8.\n");
        }
        else 
            pc.printf("pullallup does not take any arguments. It pulls all ports up in all the boards.\n");
    }
    else if(firstTk=="pullup")
    {
        if(tokens.size() ==2)
        {
            int brd = atoi(tokens.at(1).c_str());
            if(brd==1)
            {
                for(int a=12;a<=31;a++)
                setPort(dac_port1, sync1, a, 1);
                pc.printf("Pulling all ports up in board 1.\n");
            }
            else if(brd==2)
            {
                for(int a=12;a<=31;a++)
                setPort(dac_port2, sync2, a, 1);
                pc.printf("Pulling all ports up in board 2.\n");
            }
            else if(brd==3)
            {
                for(int a=12;a<=31;a++)
                setPort(dac_port3, sync3, a, 1);
                pc.printf("Pulling all ports up in board 3.\n");
            }
            else if(brd==4)
            {
                for(int a=12;a<=31;a++)
                setPort(dac_port4, sync4, a, 1);
                pc.printf("Pulling all ports up in board 4.\n");
            }
            else if(brd==5)
            {
                for(int a=12;a<=31;a++)
                setPort(dac_port5, sync5, a, 1);
                pc.printf("Pulling all ports up in board 5.\n");
            }
            else if(brd==6)
            {
                for(int a=12;a<=31;a++)
                setPort(dac_port6, sync6, a, 1);
                pc.printf("Pulling all ports up in board 6.\n");
            }
            else if(brd==7)
            {
                for(int a=12;a<=31;a++)
                setPort(dac_port7, sync7, a, 1);
                pc.printf("Pulling all ports up in board 7.\n");
            }
            else if(brd==8)
            {
                for(int a=12;a<=31;a++)
                setPort(dac_port8, sync8, a, 1);
                pc.printf("Pulling all ports up in board 8.\n");
            }
            else 
                pc.printf("The board number has to be between 1 and 8.\n");
        }
        else
            pc.printf("pulldown can take only 1 argument. pullup 6 pulls all ports up in board 6.\n");
    }
    else if(firstTk=="pullalldown")
    {
        if (tokens.size() ==1)
        {
            for(int a=12;a<=31;a++)
            {
                setPort(dac_port1, sync1, a, 0);
                setPort(dac_port2, sync2, a, 0);
                setPort(dac_port3, sync3, a, 0);
                setPort(dac_port4, sync4, a, 0);
                setPort(dac_port5, sync5, a, 0);
                setPort(dac_port6, sync6, a, 0);
                setPort(dac_port7, sync7, a, 0);
                setPort(dac_port8, sync8, a, 0);
            }
            pc.printf("Pulling all ports down in boards 1 to 8.\n");
        }
        else 
            pc.printf("pullalldown does not take any arguments. It pulls all ports down in all the boards.\n");
    }
    else if(firstTk=="pulldown")
    {
        if(tokens.size() ==2)
        {
            int brd = atoi(tokens.at(1).c_str());
            if(brd==1)
            {
                for(int a=12;a<=31;a++)
                setPort(dac_port1, sync1, a, 0);
                pc.printf("Pulling all ports down in board 1.\n");
            }
            else if(brd==2)
            {
                for(int a=12;a<=31;a++)
                setPort(dac_port2, sync2, a, 0);
                pc.printf("Pulling all ports down in board 2.\n");
            }
            else if(brd==3)
            {
                for(int a=12;a<=31;a++)
                setPort(dac_port3, sync3, a, 0);
                pc.printf("Pulling all ports down in board 3.\n");
            }
            else if(brd==4)
            {
                for(int a=12;a<=31;a++)
                setPort(dac_port4, sync4, a, 0);
                pc.printf("Pulling all ports down in board 4.\n");
            }
            else if(brd==5)
            {
                for(int a=12;a<=31;a++)
                setPort(dac_port5, sync5, a, 0);
                pc.printf("Pulling all ports down in board 5.\n");
            }
            else if(brd==6)
            {
                for(int a=12;a<=31;a++)
                setPort(dac_port6, sync6, a, 0);
                pc.printf("Pulling all ports down in board 6.\n");
            }
            else if(brd==7)
            {
                for(int a=12;a<=31;a++)
                setPort(dac_port7, sync7, a, 0);
                pc.printf("Pulling all ports down in board 7.\n");
            }
            else if(brd==8)
            {
                for(int a=12;a<=31;a++)
                setPort(dac_port8, sync8, a, 0);
                pc.printf("Pulling all ports down in board 8.\n");
            }
            else 
                pc.printf("The board number has to be between 1 and 8.\n");
        }
        else
            pc.printf("pulldown can take only 1 argument. pulldown 6 pulls all ports down in board 6.\n\n");
    }   
    else if(firstTk == "send")
    {
        if (tokens.size() ==3)
        {
            int brd = atoi(tokens.at(1).c_str());
            int myWord=0;
            myWord= strtol(tokens.at(2).c_str(), NULL, 16);
            pc.printf("WORD %d (0x%04x)\n", myWord, myWord);
            if(brd==1)
            {
                mySPISend(myWord, dac_port1, sync1);
            }
            else if(brd==2)
            {
                mySPISend(myWord, dac_port2, sync2);
            }
            else if(brd==3)
            {
                mySPISend(myWord, dac_port3, sync3);
            }
            else if(brd==4)
            {
                mySPISend(myWord, dac_port4, sync4);
            }
            else if(brd==5)
            {
                mySPISend(myWord, dac_port5, sync5);
            }
            else if(brd==6)
            {
                mySPISend(myWord, dac_port6, sync6);
            }
            else if(brd==7)
            {
                mySPISend(myWord, dac_port7, sync7);
            }
            else if(brd==8)
            {
                mySPISend(myWord, dac_port8, sync8);
            }
            else
                pc.printf("The board number has to be between 1 and 8\n");            
        }
        else
        {
            pc.printf("Send requires a board number and a hex word parameter.\ntype 'send 3 XXXX' to send the word (x must be a hex value) to board 3.\n");
        }
    }
        else if(firstTk == "read")
    {
        if (tokens.size() ==3)
        {
            int brd = atoi(tokens.at(1).c_str());
            int myWord=0;
            myWord= strtol(tokens.at(2).c_str(), NULL, 16);
            pc.printf("WORD %d (0x%04x)\n", myWord, myWord);
            if(brd==1)
                mySPIRead(pc,  myWord, dac_port1, sync1);
            else if(brd==2)
                mySPIRead(pc,  myWord, dac_port2, sync2);
            else if(brd==3)
                mySPIRead(pc,  myWord, dac_port3, sync3);
            else if(brd==4)
                mySPIRead(pc,  myWord, dac_port4, sync4);
            else if(brd==5)
                mySPIRead(pc,  myWord, dac_port5, sync5);
            else if(brd==6)
                mySPIRead(pc,  myWord, dac_port6, sync6);
            else if(brd==7)
                mySPIRead(pc,  myWord, dac_port7, sync7);
            else if(brd==8)
                mySPIRead(pc,  myWord, dac_port8, sync8);
            else
                pc.printf("The board number has to be between 1 and 8\n");            

        }
        else
        {
            pc.printf("Read requires a board number and a hex word parameter.\ntype 'send XXXX' to send the word (x must be a hex value).\n");
        }
    }
    
}