// Purpose: Get relative humidity and temperature from A2302 sensor found at akizukidenshi.com/download/ds/aosong/AM2302.pdf

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

// absolute addresses
#define GPIO0_ADDR   (0x44E07000)
#define GPIO1_ADDR   (0x4804C000)
#define GPIO2_ADDR   (0x481AC000)
#define GPIO3_ADDR   (0x481AF000)

// relative addresses (from the ones above)
#define OE_ADDR      (0x134)
#define GPIO_DATAIN  (0x138)      // read value of pins
#define GPIO_DATAOUT (0x13C)      // write value to pins

#define GPIO_PIN_NUMBER (31) // ex. if we are in GPIO0, then 30 here would mean GPIO0_30, where as GPIO1 would be GPIO1_30   
#define PIN_HIGH        (1  << GPIO_PIN_NUMBER)
#define PIN_LOW         (0)
#define PIN_INPUT(){ \
    pinconf1[OE_ADDR/4] |= (1 << GPIO_PIN_NUMBER); \
}
#define PIN_OUTPUT(){ \
    pinconf1[OE_ADDR/4] &= (0xFFFFFFFF ^ (1 << GPIO_PIN_NUMBER)); \
}
int PIN_READ(ulong* pinconf1){
    // never do this! you can still read even when it is an PIN_OUTPUT
    // and if you do this, then when you write and read it will never be
    // what you wrote the pin to be
    //PIN_INPUT(); 
    return pinconf1[GPIO_DATAIN/4] & (1  << GPIO_PIN_NUMBER);
} 

#define PIN_WRITE(val) { \
    PIN_OUTPUT(); \
    if(val == PIN_HIGH) \
    {  \
        pinconf1[GPIO_DATAOUT/4] |= (1 << GPIO_PIN_NUMBER);  \
    } \
    else if(val == PIN_LOW) \
    { \
        pinconf1[GPIO_DATAOUT/4] &= (0xFFFFFFFF ^ (1 << GPIO_PIN_NUMBER)); \
    } \
}


// state machine
#define STATE_IDLE     (0)
#define STATE_COMMAND  (1)
#define STATE_RESPONSE (2)
#define STATE_DATA     (3)
bool globalStartMeasuring = false; // used to trigger a sensor read

using namespace std;

static void startCommandHandler(int sig)
{
    globalStartMeasuring = true;
}

void print(char * message)
{
    printf("%s\n", message); 
    fflush(stdout);
}

// return:
// returns -1 if time out is greater that "timeoutUS" microseconds
// otherwise, returns the number of microseconds it took for the
// pin output became the value "pinValueToWaitFor"
// args:
// "pinValueToWaitFor" must be either PIN_HIGH or PIN_LOW
int waitForTransition(ulong *pinconf1, int pinValueToWaitFor, int timeoutUS)
{
    int countUS = 0;
    while(true)
    {
        // xxx it may be a problem that we check for this right away, because 
        // what if this is true beacuse its from the last clock cycle?
        // maybe we should wait until the pin read becomes !pinValueToWaitFor
        // THEN do this
        if(PIN_READ(pinconf1) == pinValueToWaitFor)
        {
            return countUS;
        }

        if(countUS > timeoutUS)
        {
            return -1;
        }

        countUS++;
        usleep(1);
    }
}

int main()
{
    // setup out CTRL+C signal handler
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = startCommandHandler;
    if(sigaction(SIGINT, &sa, NULL) == -1){
        print("Bad sigaction");
        return -1;
    }

    // memory-map the GPIO port directly, because using sysfs is too slow to sample at 1 us
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    ulong* pinconf1 =  (ulong*) mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO0_ADDR);

    // sample pin read
    int i = PIN_READ(pinconf1);
    printf("i=[%d]\n", i);

    // sample pin write
    PIN_WRITE(PIN_LOW);
    printf("After put low [%d]\n", PIN_READ(pinconf1));
    PIN_WRITE(PIN_HIGH);
    printf("After put high [%d]\n", PIN_READ(pinconf1));
    /*
    int val = PIN_HIGH;
    if(val == PIN_HIGH)
    { 
        pinconf1[GPIO_DATAOUT/4] |= (1 << GPIO_PIN_NUMBER); 
    }
    else if(val == PIN_LOW)
    { 
        pinconf1[GPIO_DATAOUT/4] &= (0xFFFFFFFF ^ (1 << GPIO_PIN_NUMBER)); 
    }
    */
    //*
    print("CTRL+C to Trigger sensor read.");

    // state machine
    int lastState = STATE_IDLE;
    int state = STATE_IDLE;
    int nextState = STATE_IDLE;
    while(true)
    {
        // implement the next state
        if(state != nextState)
        {
            printf("[%d] -> [%d]\n", state, nextState);
            fflush(stdout);
            lastState = state;
            state = nextState;  
        }

        // idle
        if(state == STATE_IDLE){
            if(globalStartMeasuring == true)
            {
                nextState = STATE_COMMAND;
                print("\nIdle > Command");
                globalStartMeasuring = false;
            }
        }

        // command the sensor to begin sampling
        if(state == STATE_COMMAND){
            PIN_WRITE(PIN_LOW);
            printf("After put low [%d]\n", PIN_READ(pinconf1));
            usleep(1100);
            PIN_WRITE(PIN_HIGH);
            nextState = STATE_RESPONSE;

            printf("After put high [%d]\n", PIN_READ(pinconf1));
            fflush(stdout);
        }

        // wait for the sensor to respond
        if(state == STATE_RESPONSE){
            bool validResponse = false;
            int us = waitForTransition(pinconf1, PIN_LOW, 600);
            if(us == -1){
                print("Sensor response timeout");
                nextState = STATE_IDLE;
            }else{
                printf("Waited for [%d] us\n", us);
                fflush(stdout);

                // xxx KA testing
                nextState = STATE_IDLE;
            }
        }

        //usleep(1);
        //sleep(1);
    }
    //*/

    //*
    PIN_INPUT();
    for(int i = 0; i < 250; i++)
    {
        //cout << pinconf1[GPIO_DATAIN/4]  << endl;
        //cout << PIN_READ(pinconf1)  << endl;

        // get pin GPIO_PIN_NUMBER in GPIOX
        if(PIN_READ(pinconf1) == PIN_HIGH)
        {
            cout << "high" <<endl;
        }
        else
        {
            cout << "low" <<endl;
        }

        usleep(500000);
    }
    //*/

    // ex. 0000 0010 1000 1100 0000 0001 0101 1111 1110 1110
    //     ---- ---- ---- ----                                 is RH data
    //                         ---- ---- ---- ----             is T data
    //                                             ---- ----   is checksum, where checksum = RH (2 bytes) + T (2 bytes) 

    // ex. use RH to get relative humidity
    // a) convert RH from binary to decimal
    // b) divide by 10 to get relative humidity

    // ex. use T to calculate temperature
    // a) convert T from binary to decimal
    // b) divide by 10 to get temperature



    return 0;
}