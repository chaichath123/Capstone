/*
 */
#include <p32xxxx.h>
#include <plib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Sensor Device Responses */
#define MSG_ACK 0xE // acknowledgement to the commands
#define MSG_NOTHING 0xF // reserved
#define MSG_RESET 0x0
#define MSG_PING 0x1
#define MSG_GET 0x2
#define hi = 1 // clock hi
#define lo = 0 // clock lo
#define RA2 PORTDbits.RD0
#define GO AD1CON1SET = 0x0002

#define RD0 PORTDbits.RD0// Strobe
#define RD1 PORTDbits.RD1 // PIN 49
#define RD2 PORTDbits.RD2 // Pin 50
#define RD3 PORTDbits.RD3 // Pin 51
#define RD4 PORTDbits.RD4 // pin 52
#define RD5 PORTDbits.RD5 // pin 53

unsigned int sample = 0b1111111111;
unsigned int sample2 = 1023;

int HI = 1;
int LO = 0;
unsigned char bus_buffer[3];
unsigned char adc_buffer[9];

void adc_init() {
    //1.turn off comparators
    //2.select pin as analog input
    //3.set A/D control registers
    //CMCON0 = 0x07; //CMCON Turn OFF comparators
    TRISD = 0xFFFF;
    AD1CON1SET = 0x8000; // turn the ADC on
}
unsigned char adc_read(void) {
    /*
    1.set ADC status bit to start a cycle and wait until the process is completed
    2.clear ADC interrupt flag bit
     */
    int i, temp;

    unsigned int adcValue; //used to store ADC value as integer


    for (i = 0; i < 9; i++) {
        temp = ReadADC10(1);
        adc_buffer[i] = temp;
    }

    return adcValue;
}
unsigned int conv_adc2volt(void) {
    unsigned int voltage;
    voltage = (unsigned int) adc_read() / 255;
    return voltage;
}
void send_mode() {
    TRISD = 0x0001;
    PORTD = 0x0000;
}
void receive_mode() {
    /* Pints for Galileo
     * PIN46 = RD0
     * PIN49 = RD1
     * PIN50 = RD2
     * PIN51 = RD3
     * PIN52 = RD4
     * PIN53 = RD5
     */
    TRISD = 0x003F; // 0000 0000 0011 1111
    // ANSEL = 0b00000001;
}
void ACK() {
    // R5 R4 R3 R2 R1
    // 0  1  1  1  0 = E
    RD1 = 0;
    RD2 = 1;
    RD3 = 1;
    RD4 = 1;
    RD5 = 0;
}
void motor_on() {
    TRISD = 0xFFFD; // ALL D port to input
    TRISE = 0xFFFE; // ALL D port to digital output except RD1 to digital input
    // ANSEL = 0x0000;
    AD1CON1 = 0x8000; // turn the ADC on
    /*
     while (1) {//as long as the button is not presed
        LD1 = 1; // set
        RE0 = 1; // set REO
    }
     */
}
unsigned int receive_msg() {
    // set_receive();// already been done
    unsigned int strobe_status = 0;
    strobe_status = RD0;
    return strobe_status;
}
char read_command() {
    char command = 0x0;
    RD8 = 0;
    RD9 = 0;
    RD10 = 0;
    if (RD5 == LO && RD4 == LO && RD3 == LO && RD2 == LO && RD1 == LO) {
        /*
         * ---------R E S E T------------
         */
        command = 0x1;
        RD8 = 1;
    }
    if (RD5 == LO && RD4 == LO && RD3 == LO && RD2 == LO && RD1 == HI) {
        /*
         * --------- P I N G------------
         */
        command = 0x2;
        RD9 = 1;
    }
    if (RD5 == LO && RD4 == LO && RD3 == LO && RD2 == HI && RD1 == LO) {
        //R E A D adc value
        command = 0x3;
        RD10 = 0;
        TRISD = 0x0;
        //adc_read();
        //return adc value to
    }
    return command;
}
void first_adc() {

        unsigned int adc_temp;
        //adc_temp = adc_read();
        //adc_temp = sample2;
        adc_temp = sample2;

        /*(adc_temp & (1<0))>>0*/

        if (( (adc_temp & (1 << 0))>>0 )== 1) {
            RD1 = 1;
        } else if (( (adc_temp & (1<<0) )>>0 )== 0) {
            RD1 = 0;
        }
        if (( (adc_temp & (1<< 1))>>1 )== 1) {
            RD2 = 1;
        } else if (( (adc_temp & (1<<1))>>1 )== 0) {
            RD2 = 0;
        }
        if (( (adc_temp & (1<< 2))>>2 )== 1) {
            RD3 = 1;
        } else if (( (adc_temp & (1<<2))>>2 )== 0) {
            RD3 = 0;
        }
        if (( (adc_temp & (1< <3))>>3 )== 1) {
            RD4 = 1;
        } else if (( (adc_temp & (1<<3))>>3 )== 0) {
            RD4 = 0;
        }
          if (( (adc_temp & (1<<4))>>4 )== 1) {
            RD5 = 1;
        } else if (( (adc_temp & (1<<5))>>5 )== 0) {
            RD5 = 0;
        }

}
void second_adc() {

        unsigned int adc_temp;
        //adc_temp = adc_read();
        adc_temp = sample;
        if (( (adc_temp & (1 << 5))>>5 )== 1) {
            RD1 = 1;
        } else if (( (adc_temp & (1<<5) )>>5 )== 0) {
            RD1 = 0;
        }
        if (( (adc_temp & (1<< 6))>>6 )== 1) {
            RD2 = 1;
        } else if (( (adc_temp & (1<<6))>>6 )== 0) {
            RD2 = 0;
        }
        if (( (adc_temp & (1<< 7))>>7 )== 1) {
            RD3 = 1;
        } else if (( (adc_temp & (1<<7))>>7 )== 0) {
            RD3 = 0;
        }
        if (( (adc_temp & (1< <8))>>8 )== 1) {
            RD4 = 1;
        } else if (( (adc_temp & (1<<8))>>8 )== 0) {
            RD4 = 0;
        }
          if (( (adc_temp & (1<<9))>>9 )== 1) {
            RD5 = 1;
        } else if (( (adc_temp & (1<<9))>>9 )== 0) {
            RD5 = 0;
        }

}

void main() {
    unsigned int S = 0;
    unsigned int PING = 0;
    unsigned int READ = 0;
    unsigned int RESET = 0;
    char command = 0;
    
    adc_init();
    while (1) {
        PING = 0;
        READ = 0;
        RESET = 0;
        S = receive_msg();

        while (S == HI) {//GALILEO ALWAYS in High state
            ; //Read ADC VALUE
        }
        while (S == LO) {
            ;// GALILEO WRITE
        }
        while (S == HI) {
            command = read_command();
        }
        while (S == LO) {

            switch (command) {
                case 0x0:
                    break;
                case 0x1: //RESET

                    ACK();
                    adc_init();
                    break;
                case 0x2: // ping
                    //(); //reset
                    send_mode();
                    while( S == HI);
                   // TRISD = 0xFFC1;
                     // RC is output
                 
                    break;

                case 0x3: //get ADC
                    send_mode();//
                    first_adc();

                    while( S == HI){//send first 5
                        
                    }
                    receive_mode();
                    while(S  == LO){//send second
                    second_adc();
                    }
                    break;

                default:
                    break;

            }
        }
        while(S== HI){
            //wati for Galileo to read
        }

        receive_mode(); // back to wating for command from galileo

    }

    return (EXIT_SUCCESS);
}
