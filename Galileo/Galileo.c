/* 	
	By Chaichath Wiyarand
	
	Description:
  In this galileo, the connection between PIC32 and Galileo will be established. The simple acknowledgement will created using
  5 data line, instead of 4.
 */

// include files
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

// LIST OF DATA BUSSES and STROBE
#define Strobe (19) // IO9
#define GP_4 (28) // IO4
#define GP_5 (17) // IO5
#define GP_6 (24) // IO6
#define GP_7 (27) //GPIO27 corresponds to Arduino PIN7
#define GP_8 (26) // IO8

#define GPIO_DIRECTION_IN (1) //input
#define GPIO_DIRECTION_OUT (0) // output
#define GPIO_STRONG (1) // PULL HIGH
#define GPIO_PULLDOWN (0) // PULL LOW


#define ERROR (-1)
#define BLINK_TIME_SEC (1)


int fileHandleGPIO_4;
int fileHandleGPIO_5;
int fileHandleGPIO_6;
int fileHandleGPIO_7;
int fileHandleGPIO_8;
int fileHandleGPIO_S;


// for setting the direction
// takes the gpio input and direction input
/* ---------- O P E N _ G P I O --------------- */

int openGPIO(int gpio, int direction)
{
			// variable initialization
	        char buffer[256];
			// char buffer2[256];
	        int fileHandle;
	        int fileMode;
			
			// Export GPIO
	        fileHandle = open("/sys/class/gpio/export", O_WRONLY);//export GPIO to "fileHandle", write only 
	        if(ERROR == fileHandle)
	        {
	               puts("Error: Unable to opening /sys/class/gpio/export");
	               return(-1);
	        }
			
	        sprintf(buffer, "%d", gpio);
	        write(fileHandle, buffer, strlen(buffer));
	        close(fileHandle);
	 
			// Setting Direction of the GPIOs
	        sprintf(buffer, "/sys/class/gpio/gpio%d/direction", gpio);
	        fileHandle = open(buffer, O_WRONLY );
	        if(ERROR == fileHandle) // If error opening the file
	        {
	               puts("Unable to open file:");
	               puts(buffer);
	               return(-1);
	        }
			
			// if the the GPIO direction is set to output
	        if (direction == GPIO_DIRECTION_OUT)
	        {
	               // Set out direction to output
				   // int  write(  int  handle,  void  *buffer,  int  nbyte  );
	               write(fileHandle, "out", 3); 
	               fileMode = O_WRONLY ; // set file mode to write only
	        }
	        else // if not, then it has to be input
	        {
	               // Set in direction to input
	               write(fileHandle, "in", 2);
	               fileMode = O_RDONLY ; // set the file mode to read only
	        }
			 close(fileHandle);
	 
            // Open GPIO for Read / Write
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

/* ---------- C L O S E _ G P I O --------------- */

int closeGPIO(int gpio, int fileHandle)
{
	char buffer[256];
	close(fileHandle); //This is the file handle of opened GPIO for Read / Write earlier.
	fileHandle = open("/sys/class/gpio/unexport", O_WRONLY);

	if(ERROR == fileHandle)
	{
		puts("Unable to open file:");
		puts(buffer);
		return(-1);
	}
	
	sprintf(buffer, "%d", gpio);
	write(fileHandle, buffer, strlen(buffer));
	close(fileHandle);
	return(0);
}

/* ---------- G P I O _ I N ---------------
 */
void GPIO_IN(void)
{
	fileHandleGPIO_4 = openGPIO(GP_4, GPIO_DIRECTION_IN);//set port 4 to send out
	fileHandleGPIO_5 = openGPIO(GP_5, GPIO_DIRECTION_IN);//set port 5 to send out
	fileHandleGPIO_6 = openGPIO(GP_6, GPIO_DIRECTION_IN);//set port 6 to send out
	fileHandleGPIO_7 = openGPIO(GP_7, GPIO_DIRECTION_IN);//set port 7 to send out
	fileHandleGPIO_8 = openGPIO(GP_8, GPIO_DIRECTION_IN);//set port 8 to send out
}


/* ---------- G P I O _ I N --------------- */
void GPIO_OUT(void)
{
	fileHandleGPIO_4 = openGPIO(GP_4, GPIO_DIRECTION_OUT);
	fileHandleGPIO_5 = openGPIO(GP_5, GPIO_DIRECTION_OUT);
	fileHandleGPIO_6 = openGPIO(GP_6, GPIO_DIRECTION_OUT);
	fileHandleGPIO_7 = openGPIO(GP_7, GPIO_DIRECTION_OUT);
	fileHandleGPIO_8 = openGPIO(GP_8, GPIO_DIRECTION_OUT);
	fileHandleGPIO_S = openGPIO(Strobe, GPIO_DIRECTION_OUT);
}


/* ---------- M S G _ P I N G --------------- */
void MSG_PING(void)
{
	printf("\n\nWriting PING 0x1 data on bus\n\n");
	writeGPIO(fileHandleGPIO_4, 1); //write high
	writeGPIO(fileHandleGPIO_5, 0); //write low
	writeGPIO(fileHandleGPIO_6, 0); //write low
	writeGPIO(fileHandleGPIO_7, 0); //write low		
	writeGPIO(fileHandleGPIO_8, 0); //write low
}


/* ---------- M S G _ R E S E T --------------- */
void MSG_RESET(void)
{
	printf("\n\nWriting RESET 0x0 data on bus\n\n");
	writeGPIO(fileHandleGPIO_4, 0); //write low
	writeGPIO(fileHandleGPIO_5, 0); //write low
	writeGPIO(fileHandleGPIO_6, 0); //write low
	writeGPIO(fileHandleGPIO_7, 0); //write low
	writeGPIO(fileHandleGPIO_8, 0); //write low
}


/* ---------- M S G _ G E T --------------- */
void MSG_GET(void)
{
	printf("\n\nWriting GET 0x2 data on bus\n\n");
	writeGPIO(fileHandleGPIO_4, 0); //write low
	writeGPIO(fileHandleGPIO_5, 1); //write low
	writeGPIO(fileHandleGPIO_6, 0); //write low
	writeGPIO(fileHandleGPIO_7, 0); //write low
	writeGPIO(fileHandleGPIO_8, 0); //write low
}


/* ---------- D E L A Y --------------- */
void delay(int milliseconds)
{
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();
}


/* ---------- R E A D _ G P I O --------------- */
int readGPIO(int fileHandle)
	{
	lseek(fileHandle, 0, SEEK_SET);
	GPIO_IN();//switch input in
	delay(50);
	int value;
	read(fileHandle, &value , 1);
	if('0' == value)
	{
		// Current GPIO status low
		value = 0;
	}
	else
	{
		// Current GPIO status high
		value = 1;
	}
	return value;
}


/* ---------- W R I T E _ G P I O --------------- */
int writeGPIO(int fHandle, int val)
{
	GPIO_OUT();
		// lseek(fHandle, 0, SEEK_SET);
		if(val == 0)
		{
		// Set GPIO low status
		write(fHandle, "0", 1);
		}
		else
		{
		// Set GPIO high status
		write(fHandle, "1", 1);
		}
	return(0);
}


/* ---------- R E C E I V E _ A C K --------------- */
void Receive_ACK(void)
{
	if(readGPIO(fileHandleGPIO_4) == 0 && readGPIO(fileHandleGPIO_5) == 1 && readGPIO(fileHandleGPIO_6) == 1 && readGPIO(fileHandleGPIO_7) == 1 && readGPIO(fileHandleGPIO_8) == 0);
	{
		printf("\n\n****** ACKNOWLEDGE FROM PIC ******\n\n");
	}
	
	if(readGPIO(fileHandleGPIO_4)  == 0)
	{
	  printf("W\n");
	}
	
	
	//CHECK IF ALL BITS ARE AS EXPECTED
	printf("%u\n", readGPIO(fileHandleGPIO_4));// bit for GPIO4
	printf("%u\n", readGPIO(fileHandleGPIO_5));// bit for GPIO5
	printf("%u\n", readGPIO(fileHandleGPIO_6));// bit for GPIO6
	printf("%u\n", readGPIO(fileHandleGPIO_7));// bit for GPIO7
	printf("%u\n", readGPIO(fileHandleGPIO_8));// bit for GPIO8
	
}


/* ---------- R E C E I V E _ A D C --------------- */
void Receive_ADC(int x, unsigned int bit)
{
	unsigned int bit0 = readGPIO(fileHandleGPIO_4);
	unsigned int bit1 = readGPIO(fileHandleGPIO_5);
	unsigned int bit2 = readGPIO(fileHandleGPIO_6);
	unsigned int bit3 = readGPIO(fileHandleGPIO_7);
	unsigned int bit4 = readGPIO(fileHandleGPIO_8);
	// printf("%d\n", bit0);
	// printf("%d\n", bit1);
	// printf("%d\n", bit2);
	// printf("%d\n", bit3);
	
	if (x < 10)
	{
		bit = (((bit3 << (x-1)))) | bit;
		bit = (((bit2 << (x-2)))) | bit;
		bit = (((bit1 << (x-3)))) | bit;
		bit = (((bit0 << (x-4)))) | bit;
		printf("ADC Val: %u\n", bit);
	}
	if (x > 10)
	{
		bit = (((bit1 << (x-2)))) | bit;
		bit = (((bit0 << (x-3)))) | bit;
		printf("ADC Val: %u<<<-------\n\n", bit);
		
	}
}


/* ----------M A I N --------------- */
int main(void)
{
	int i=0;
	puts("Starting program on Galileo board.");
	int input;
	int val = 1;

	while(val)
	{
	GPIO_OUT();//set GPIO as output
	
	printf( "0. Power LED\n" );
    printf( "1. Reset Sensor\n" );
    printf( "2. PING Sensor\n" );
	printf( "3. ADC Read\n" );
    printf( "4. Exit\n" );
    printf( "Selection: " );
    scanf( "%d", &input );
    switch ( input ) {
        case 0:            
            writeGPIO(fileHandleGPIO_S, 0); //pull low
			sleep(5);
			writeGPIO(fileHandleGPIO_S, 1); //pull high
            break;
        case 1:    //Reset                                                                                                                                                                                          v
			// Strobe
			printf("\n\nSTROBE LOW\n\n");
			writeGPIO(fileHandleGPIO_S, 0); //pull low pic waits
			// write reset data on bus
			MSG_RESET();
			delay(50);
			printf("\n\nSTROBE HIGH\n\n");
			writeGPIO(fileHandleGPIO_S, 1); //pull high now pic reads
			printf("\n\nSTROBE LOW GALILEO WAITING\n\n");
			writeGPIO(fileHandleGPIO_S, 0); //pull low gal waits
			// GPIO_IN(); // set GPIO as input
			delay(50);
			printf("\n\nSTROBE HIGH GALILEO now reading\n\n");
			writeGPIO(fileHandleGPIO_S, 1); //pull high now gal reads
			// GPIO_IN();
			Receive_ACK();//check if receive ack
            break;
        case 2:         //PING
			// Strobe
			printf("\n\nSTROBE LOW\n\n");
			writeGPIO(fileHandleGPIO_S, 0); //pull low pic waits
			// write data on bus
			MSG_PING();
			delay(50);
			printf("\n\nSTROBE HIGH\n\n");
			writeGPIO(fileHandleGPIO_S, 1); //pull high now pic reads
			printf("\n\nSTROBE LOW GALILEO WAITING\n\n");
			writeGPIO(fileHandleGPIO_S, 0); //pull low gal waits
			// GPIO_IN();
			delay(50);
			printf("\n\nSTROBE HIGH GALILEO now reading\n\n");
			writeGPIO(fileHandleGPIO_S, 1); //pull high now gal reads
			Receive_ACK();
            break;
		case 3:   //ADC
         // Strobe
			printf("\n\nSTROBE LOW\n\n");
			writeGPIO(fileHandleGPIO_S, 0); //pull low pic waits
			// write data on bus
			unsigned int bits = 0x0000000000;
			MSG_GET();//adc
			delay(50);
			printf("\n\nSTROBE HIGH\n\n");
			writeGPIO(fileHandleGPIO_S, 1); //pull high now pic reads
			printf("\n\nSTROBE LOW GALILEO WAITING\n\n");
			writeGPIO(fileHandleGPIO_S, 0); //pull low gal waits
			delay(6000);
			printf("\n\nSTROBE HIGH 1st nibble\n\n");
			writeGPIO(fileHandleGPIO_S, 1); //pull high now gal reads
			Receive_ADC(4, bits);
			writeGPIO(fileHandleGPIO_S, 0); //pull low gal waits
			delay(4500);
			printf("\n\nSTROBE HIGH 2nd nibble\n\n");
			writeGPIO(fileHandleGPIO_S, 1); //pull high now gal reads
			Receive_ADC(8, bits);
			writeGPIO(fileHandleGPIO_S, 0); //pull low gal waits
			delay(6000);
			printf("\n\nSTROBE HIGH 3rd nibble\n\n");
			writeGPIO(fileHandleGPIO_S, 1); //pull high now gal reads
			Receive_ADC(12, bits);
		/* 	int i,j;
			for (i=0;i<3;i++){
				writeGPIO(fileHandleGPIO_S, 1); //pull high now pic reads
				printf("STROBE LOW GALILEO  WAITING\n\n");
				writeGPIO(fileHandleGPIO_S, 0); //pull low gal waits
			} */
			// Receive_ACK();
			
            break;
        case 4:        
            printf( "Thanks bye\n" );
			val = 0;
            break;
        default:            
            printf( "Bad input, quitting!\n" );
            break;
		}
		
	closeGPIO(GP_7, fileHandleGPIO_7);
	closeGPIO(GP_6, fileHandleGPIO_6);
	closeGPIO(GP_5, fileHandleGPIO_5);
	closeGPIO(GP_4, fileHandleGPIO_4);
	closeGPIO(Strobe, fileHandleGPIO_S);
	}
		
	closeGPIO(GP_7, fileHandleGPIO_7);
	closeGPIO(GP_6, fileHandleGPIO_6);
	closeGPIO(GP_5, fileHandleGPIO_5);
	closeGPIO(GP_4, fileHandleGPIO_4);
	closeGPIO(Strobe, fileHandleGPIO_S);
	
	puts("Finished program on Galileo board.");
	
	return 0;
}
