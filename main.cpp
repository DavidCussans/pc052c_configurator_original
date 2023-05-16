//pc052c_configurator
//S.V. Addepalli & P. Baesso  - sagar.addepalli@iitb.ac.in
//This firmware can be installed on a KL25Z board and provides
//a serial interface to configure the pc052c board.
//The communication between KL25Z and pc052c uses the SPI interface.
// Original before porting to BufferedSerial and tidying UI
// David Cussans, April 2023

#include "mbed.h"
#include "led_options.h"
#include "spi_functions.h"
#include <vector>
#include <string> 
#include <map>

std::vector<std::string> tokenize(const std::string& str, const std::string& delim);
void doCommands(std::vector<std::string>& tokens);
using namespace std;

// DEFINE SERIAL INTERFACE AND PINS USED BY THE SPI PROTOCOL
//Serial pc(USccBTX, USBRX);
BufferedSerial pc(USBTX, USBRX); // tx, rx
DigitalOut myled(LED1);
SPI dac_port1(PTD2, PTD3, PTD1); //mosi, miso, sclk //SPI0
SPI dac_port2(PTD2, PTC6, PTD1);  //SPI0
SPI dac_port3(PTD2, PTC7 , PTD1);    //SPI0 -- clashes with Rx
SPI dac_port4(PTD2, PTA16, PTD1); //SPI0
SPI dac_port5(PTD2, PTA17, PTD1); //SPI0
SPI dac_port6(PTE3, PTD7, PTE2);    //SPI1
SPI dac_port7(PTE3, PTE1, PTE2);    //SPI1
SPI dac_port8(PTE3, PTD6, PTE2);    //SPI1
DigitalOut sync1(PTD5, 1);// Board 1 slave select. Initialize to high 
DigitalOut sync2(PTD0, 1); // Board 2 slave select. Initialize to high good to go
DigitalOut sync3(PTE4, 1); // Board 3 slave select. Initialize to high GOOD TO GO
//DigitalOut sync3(PTE5, 1); // Board 3 slave select. Initialize to high GOOD TO GO
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

std::map<int,SPI*> board_dac_port_map = {
    {1,&dac_port1},
    {2,&dac_port2},
    {3,&dac_port3},
    {4,&dac_port4},
    {5,&dac_port5},
    {6,&dac_port6},
    {7,&dac_port7},
    {8,&dac_port8}
    };

std::map<int,DigitalOut*> board_sync_map = 
{
    {1,&sync1},
    {2,&sync2},
    {3,&sync3},
    {4,&sync4},
    {5,&sync5},
    {6,&sync6},
    {7,&sync7},
    {8,&sync8}
    };

bool scan_mode;

int main()
{
    printf("\npc052c Configurator v0.12\n");
    printf("\nStarting interface.\n");
    printf("Enter commands and press TAB to send.\n");
    printf("Type 'help'+TAB for a list of commands.\n");
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

    scan_mode=false;
    
    //Initialize variable used to select cases
    char cUser;
    while(runFlag)
    {
        if(pc.readable())
        {
            //printf("Entering reading condition\n");
            pc.read(&cUser, 1);
            if (cUser== '\t')// \n"
            {
                //printf("End of line detected. Parsing command.\n");
                
                //PARSE COMMANDS
                string delim= " ";
                vector<string> myTokens = tokenize(myCommand, delim);
                
                //printf("%d TOKENS FOUND.\n", myTokens.size());
                //printf("TOKENS= ");
                //for (int iTk=0; iTk <myTokens.size(); iTk++)
                //{
                //    printf("%s - ", myTokens.at(iTk).c_str());
                //}
                //printf("\n");
                doCommands(myTokens);

                   
                //RESET WORD
                myCommand= "";
            }
            else
            {
                myCommand= myCommand+ cUser;
                if (!scan_mode)
                    printf("%s\n", myCommand.c_str());
            }
        }
        
        
    }
    printf("Interface stopped. Press the reset button to start again.\n");
    wait_us(1000000);

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
        printf("Stopping interface.\n");
        runFlag=0;
    }
    else if(firstTk == "help") //Help
    {
        printf("**Type commands and press TAB to execute them.\n Valid commands are:\n'exit'\n'help'\n'powermode'\n'configure'\n'setport'\n'send'\n'pullallup'\n'pullalldown'\n'pulldown'\n'pullup'\n'scanmode'\n\n");
    }
    else if(firstTk == "configure")//Send configuration
    {
        if (!scan_mode)
            printf("Configuring all ports as outputs (low).\n");
        sendConfiguration(dac_port1, sync1, scan_mode);
        sendConfiguration(dac_port2, sync2, scan_mode);
        sendConfiguration(dac_port3, sync3, scan_mode);
        sendConfiguration(dac_port4, sync4, scan_mode);
        sendConfiguration(dac_port5, sync5, scan_mode);
        sendConfiguration(dac_port6, sync6, scan_mode);
        sendConfiguration(dac_port7, sync7, scan_mode);
        sendConfiguration(dac_port8, sync8, scan_mode);
        printf("Configuration done.\n");
    }
    else if(firstTk == "powermode")//Set the specific board's power mode (shutdown or active)
    {

        if (tokens.size() ==3)
        {   
            int brd = atoi(tokens.at(1).c_str());
            int pMode = atoi(tokens.at(2).c_str());
            if ((pMode==0) || (pMode==1))
            {
                if (board_dac_port_map.count(brd))
                { 
                    powerMode(*board_dac_port_map.at(brd), *board_sync_map.at(brd), pMode);
                    printf("Power mode for board %d = %d.\n", brd, pMode);
                }
                else
                    printf("The board number has to be between 1 and 8\n");
            }
            else
            {
                printf("This function can only accept 0 or 1. You typed %d.\n", pMode);
            }
        }
        else
        {
            printf("Powermode command requires 2 parameters:\ntype 'powermode brdno 1' to power up the MAX6956\ntype 'powermode brdno 0' to shut it down.\n");    
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
                if (board_dac_port_map.count(brd))
                {   
                    setPort(*board_dac_port_map.at(brd), *board_sync_map.at(brd), portN, portState);
                    printf("Port %d board %d set to %d.\n", portN, brd, portState);
                }
                else
                {
                    printf("The board number has to be between 1 and 8\n");
                }
                //toWrite=(portN + 0x0020)*0x0100 + portState;
                //printf("WORD %d (0x%04x)\n", toWrite, toWrite);
            }
            else
            {
                printf("Valid ports number: 12 - 31. Valid status: 0 - 1. You typed port %d state %d.\n", portN, portState);
            }
            
        }
        else
        {
            printf("Setport requires 3 parameters:\ntype 'setport 5 21 1' to set port 21 on board 5 as 'high'\ntype 'setport 5 21 0' to set it 'low'.\nThis command assumes that the port is already configured as output.\n");    
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
            printf("Pulled all ports up in boards 1 to 8.\n");
        }
        else 
            printf("pullallup does not take any arguments. It pulls all ports up in all the boards.\n");
    }
    else if(firstTk=="pullup")
    {
        if(tokens.size() ==2)
        {
            int brd = atoi(tokens.at(1).c_str());

            if (board_dac_port_map.count(brd))
            {
                for(int a=12;a<=31;a++)
                {
                    setPort(*board_dac_port_map.at(brd), *board_sync_map.at(brd), a, 1);
                }
                printf("Pulled all ports up in board %d.\n",brd);
            }
            else 
                printf("The board number has to be between 1 and 8.\n");
        }
        else
            printf("pullup can take only 1 argument. pullup 6 pulls all ports up in board 6.\n");
    }
    else if(firstTk=="pullalldown")
    {
        if (tokens.size() ==1)
        {
            //printf("Pulling all ports down in boards 1 to 8.\n");
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
            printf("Pulled all ports down in boards 1 to 8.\n");
        }
        else 
            printf("pullalldown does not take any arguments. It pulls all ports down in all the boards.\n");
    }
    else if(firstTk=="pulldown")
    {
        if(tokens.size() ==2)
        {
            int brd = atoi(tokens.at(1).c_str());
            if (board_dac_port_map.count(brd))
            {
                for(int a=12;a<=31;a++)
                {
                    setPort(*board_dac_port_map.at(brd), *board_sync_map.at(brd), a, 0);
                }
                printf("Pulled all ports down in board %d.\n",brd);
            }
            else 
                printf("The board number has to be between 1 and 8.\n");
        }
        else
            printf("pulldown can take only 1 argument. pulldown 6 pulls all ports down in board 6.\n");
    }   
    else if(firstTk == "send")
    {
        if (tokens.size() ==3)
        {
            int brd = atoi(tokens.at(1).c_str());
            int myWord=0;
            myWord= strtol(tokens.at(2).c_str(), NULL, 16);
            printf("WORD %d (0x%04x)\n", myWord, myWord);
            if (board_dac_port_map.count(brd))
            {
                mySPISend(myWord, *board_dac_port_map.at(brd), *board_sync_map.at(brd));
            }
            else
                printf("The board number has to be between 1 and 8\n");            
        }
        else
        {
            printf("Send requires a board number and a hex word parameter.\ntype 'send 3 XXXX' to send the word (x must be a hex value) to board 3.\n");
        }
    }
    else if(firstTk == "read")
    {
        if (tokens.size() ==3)
        {
            int brd = atoi(tokens.at(1).c_str());
            int myWord=0;
            myWord= strtol(tokens.at(2).c_str(), NULL, 16);
            printf("WORD %d (0x%04x)\n", myWord, myWord);
            if (board_dac_port_map.count(brd))
            {
                mySPIRead(myWord, *board_dac_port_map.at(brd), *board_sync_map.at(brd), scan_mode);
            }
            else
            {
                printf("The board number has to be between 1 and 8\n");        
            }
        }
        else
        {
            printf("Read requires a board number and a hex word parameter.\ntype 'send XXXX' to send the word (x must be a hex value).\n");
        }
    }
    else if(firstTk=="scanmode")
    {
        if(tokens.size() ==2)
        {
            int on_off = atoi(tokens.at(1).c_str());
            if (on_off == 0)
            {
                scan_mode = 0;
                printf("Scan mode off.\n");
            }
            else if (on_off == 1)
            {
                scan_mode = 1;
                printf("Scan mode on.\n");
            }
            else 
                printf("The scan mode needs 1 or 0.\n");
        }
        else
            printf("scanmode can take only 1 argument, 0 or 1.\n");
    }
    else {
    {
        printf("Bad command: %s\n", firstTk.c_str());
    }
    }
}