/* 	
	By Chaichath Wiyarand
	
	Description:
  In this galileo, the connection between PIC32 and Galileo will be established. The simple acknowledgement will created using
  5 data line, instead of 4..
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <pthread.h>
#include <curl/curl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

/* user commands */
#define MSG_RESET	0x0  /* reset the sensor to initial state */
#define MSG_PING	0x1  /* check ifx the sensor is working properly*/
#define MSG_GET		0x2  /* obtain the most recent ADC result */
#define Strobe 		(4) // IO9 to pin 46
#define GP_4       	(6)// IO4
#define GP_5	  	(0) // IO5
#define GP_6	  	(1) // IO6
#define GP_7	  	(38) // IO7
#define GP_8     	(40) // IO8
#define GPIO_DIRECTION_IN      (1)
#define GPIO_DIRECTION_OUT     (0)
#define ERROR                  (-1)
#define delayTime (10000) // usleep() delay time can be set here.
int fileHandleGPIO_4; // pin 4 (LSB) of GAL mapped to pin 10 (C0) of PIC
int fileHandleGPIO_5; // pin 5 of GAL mapped to pin 9 (C1) of PIC
int fileHandleGPIO_6; // pin 6 of GAL mapped to pin 8 (C2) of PIC
int fileHandleGPIO_7; // pin 7 (MSB) of GAL mapped to pin 7 (C3) of PIC
int fileHandleGPIO_8; // pin 8
int fileHandleGPIO_S; // pin 8 (strobe) of GAL mapped to pin 11 (RA2) of PIC


void delay(unsigned int dly) {
    unsigned int temp;
    for (; dly > 0; dly--)
        for (temp = 1000; temp > 0; temp--);
}


int openFileForReading(gpio)
{
    char buffer[256];
    int fileHandle;
    sprintf(buffer, "/sys/class/gpio/gpio%d/value", gpio);
    fileHandle = open(buffer, O_RDONLY);
    if(ERROR == fileHandle)
    {
        puts("Unable to open file:");
        puts(buffer);
        return(-1);
    }
    return(fileHandle);  //This file handle will be used in read/write and close operations.
}

/*  ---------- O P E N _ G P I O ---------------
	Description : These steps are implemented using the openGPIO function, 
				which opens the corresponding file and 
				returns this file identifier for future reading or writing, 
				depending on the direction declared.
				
	Return: fileHandle or the GPIO
*/
int openGPIO(int gpio, int direction)
{
    //1.set the GPIO
    //2.set the direction
    //3.set the voltage
    char buffer[256];
    int fileHandle;
    int fileMode;
    //Export GPIO
    fileHandle = open("/sys/class/gpio/export", O_WRONLY);
    if(ERROR == fileHandle)
    {
        puts("Error: Unable to open /sys/class/gpio/export");
        return(-1);
    }
    sprintf(buffer, "%d", gpio);
    write(fileHandle, buffer, strlen(buffer));
    close(fileHandle);
    //Direction GPIO
    sprintf(buffer, "/sys/class/gpio/gpio%d/direction", gpio);
    fileHandle = open(buffer, O_WRONLY);
    if(ERROR == fileHandle)
    {
        puts("Unable to open file:");
        puts(buffer);
        return(-1);
    }
    if (direction == GPIO_DIRECTION_OUT)
    {
        // Set out direction
        write(fileHandle, "out", 3);
        fileMode = O_WRONLY;
    }
    else
    {
        // Set in direction
        write(fileHandle, "in", 2);
        fileMode = O_RDONLY;
    }
    close(fileHandle);
    //Drive GPIO
    sprintf(buffer, "/sys/class/gpio/gpio%d/drive", gpio);
    fileHandle = open(buffer, O_WRONLY);
    if(ERROR == fileHandle)
    {
        puts("Unable to open file:");
        puts(buffer);
        return(-1);
    }
    if (direction == GPIO_DIRECTION_OUT)
    {
        // Set strong
        write(fileHandle, "strong", 6);
    }
    else
    {
        // Set hi-Z
        write(fileHandle, "hiz", 3);
    }
    close(fileHandle);
    //Open GPIO for Read / Write
    sprintf(buffer, "/sys/class/gpio/gpio%d/value", gpio);
    fileHandle = open(buffer, fileMode);
    if(ERROR == fileHandle)
    {
        puts("Unable to open file:");
        puts(buffer);
        return(-1);
    }
    return(fileHandle);  //This file handle will be used in read/write and close operations.
}
/*---------- R E A D _ G P I O --------------- 
	Description: The GPIO reading logic is as follows.
	
	Return: digital value for that GPIO
*/
int readGPIO(int fHandle, int gpio)
{
    int value;
	//Reopening the file again in read mode, since data was not refreshing.
    fHandle = openFileForReading(gpio); // open file
    read(fHandle, &value, 1); //read the digital value from the pin
    if('0' == value){// if 0
        value = 0;       // Current GPIO status low
    }else{// else has to be high
        value = 1;       // Current GPIO status high
    }
    close(fHandle); // close the file
    return value;
}

/* ---------- W R I T E _ G P I O --------------- 
	Description:	Manually set the GPIO of the pin
				to either 1 or 0;
	
	Return nothing
	
*/
int writeGPIO(int fHandle, int val)
{
    if(val ==  0){
        write(fHandle, "0", 1); // Set GPIO to output 0
    }else{
        write(fHandle, "1", 1); // Set GPIO to output 1
    }
    return(0);
}
/* ---------- G P I O _ I N --------------- 
	Description: Setting the value for GPIO to READING MODE
		ready to receive input from PIC	
	Return nothing
*/
void GPIO_IN(void)
{
	fileHandleGPIO_4 = openGPIO(GP_4, GPIO_DIRECTION_IN);//set port 4 to receive
	fileHandleGPIO_5 = openGPIO(GP_5, GPIO_DIRECTION_IN);//set port 5 to receive
	fileHandleGPIO_6 = openGPIO(GP_6, GPIO_DIRECTION_IN);//set port 6 to receive
	fileHandleGPIO_7 = openGPIO(GP_7, GPIO_DIRECTION_IN);//set port 7 to receive
	fileHandleGPIO_8 = openGPIO(GP_8, GPIO_DIRECTION_IN);//set port 8 to receive 
}
/* ---------- G P I O _ I N --------------- 
	Description: Setting the value for GPIO to WRITING MODE
		ready to write the output to PIC
	Return nothing*/
void GPIO_OUT(void)
{
	fileHandleGPIO_4 = openGPIO(GP_4, GPIO_DIRECTION_OUT);//set port 4 to send 
	fileHandleGPIO_5 = openGPIO(GP_5, GPIO_DIRECTION_OUT);//set port 5 to send 
	fileHandleGPIO_6 = openGPIO(GP_6, GPIO_DIRECTION_OUT);//set port 6 to send 
	fileHandleGPIO_7 = openGPIO(GP_7, GPIO_DIRECTION_OUT);//set port 7 to send 
	fileHandleGPIO_8 = openGPIO(GP_8, GPIO_DIRECTION_OUT);//set port 8 to send 
	fileHandleGPIO_S = openGPIO(Strobe, GPIO_DIRECTION_OUT);
}

void main(){
	
	GPIO_IN(); // set pin4-8 tp input mode
	/*Initialization*/
	unsigned int pin8; 
	unsigned int pin7;
	unsigned int pin6;
	unsigned int pin5;
	unsigned int pin4;
	while(1){
		readGPIO(fileHandleGPIO_4, GP_4);//get the digital value from GPIO # 4
		readGPIO(fileHandleGPIO_5, GP_5);//get the digital value from GPIO # 5
		readGPIO(fileHandleGPIO_6, GP_6);//get the digital value from GPIO # 6
		readGPIO(fileHandleGPIO_7, GP_7);//get the digital value from GPIO # 7
		readGPIO(fileHandleGPIO_8, GP_8);//get the digital value from GPIO # 8
		pin4 = readGPIO(fileHandleGPIO_4, GP_4);//store value into pin4
		pin5 = readGPIO(fileHandleGPIO_5, GP_5);//store value into pin5
		pin6 = readGPIO(fileHandleGPIO_6, GP_6);//store value into pin6
		pin7 = readGPIO(fileHandleGPIO_7, GP_7);//store value into pin7
		pin8 = readGPIO(fileHandleGPIO_8, GP_8);//store value into pin8
		
		//TEST if GPIO are all outputing the correct value
		printf("%u %u %u %u %u\n", readGPIO(fileHandleGPIO_4, GP_4),readGPIO(fileHandleGPIO_5, GP_5),	readGPIO(fileHandleGPIO_6, GP_6),	readGPIO(fileHandleGPIO_7, GP_7),	readGPIO(fileHandleGPIO_8, GP_8));
		if(pin4 == 1 && pin5 == 1 && pin6 == 0 && pin7 == 1 && pin8 == 1){
			printf("HELLO CRUEL WORLD!");
		}
		delay(2000);
		
	}
}
