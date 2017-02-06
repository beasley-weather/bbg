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

// am2302
#define NUM_BITS_DATA     (40)
#define NUM_BITS_RH       (16)
#define NUM_BITS_T        (16)
#define NUM_BITS_SUM      (8)
#define CONVERSION_FACTOR (10.0) // divide by 10

// state machine
#define STATE_IDLE     (0)
#define STATE_COMMAND  (1)
#define STATE_RESPONSE (2)
#define STATE_DATA     (3)
#define STATE_CALC     (4)
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
    PIN_INPUT();

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
    // setup CTRL+C signal handler
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
    int data[NUM_BITS_DATA];
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
        if(state == STATE_IDLE)
        {
            if(globalStartMeasuring == true)
            {
                // clear data
                for(int x = 0; x < NUM_BITS_DATA; x++)
                {
                    data[x] = 0;
                }

                nextState = STATE_COMMAND;
                print("\nIdle > Command");
                globalStartMeasuring = false;
            }
        }

        // command the sensor to begin sampling
        if(state == STATE_COMMAND)
        {
            PIN_WRITE(PIN_LOW);
            printf("After put low [%d]\n", PIN_READ(pinconf1));
            usleep(19000);
            PIN_WRITE(PIN_HIGH);
            nextState = STATE_RESPONSE;

            printf("After put high [%d]\n", PIN_READ(pinconf1));
            fflush(stdout);
        }

        // wait for the sensor to respond
        if(state == STATE_RESPONSE)
        {
            // bbg waits for 20-40 us
            int us = waitForTransition(pinconf1, PIN_LOW, 60);
            if(us == -1){
                print("Sensor low response timeout");
                nextState = STATE_IDLE;
                continue;
            }else{
                printf("Waited for [%d] us\n", us);
                fflush(stdout);

                // xxx KA testing
                nextState = STATE_IDLE;
            }

            // sensor pulls low for 80 us
            us = waitForTransition(pinconf1, PIN_HIGH, 100);
            if(us == -1){
                print("Sensor high response timeout");
                nextState = STATE_IDLE;
                continue;
            }

            nextState = STATE_DATA;
        }

        //*
        // gather the data from the sensor
        if(state == STATE_DATA)
        {
            // sensor stays high for 80 us
            int us = waitForTransition(pinconf1, PIN_LOW, 100);
            if(us == -1){
                print("Sensor low 2 response timeout");
                nextState = STATE_IDLE;
                continue;
            }

            int x = 0;
            for(x = 0; x < NUM_BITS_DATA; x++)
            {
                // sensor goes low for 50 us to indicate start of bit
                us = waitForTransition(pinconf1, PIN_HIGH, 60);
                if(us == -1){
                    print("Sensor startbit response timeout");
                    nextState = STATE_IDLE;
                    break;
                } 

                // duration of high indicates whether it is a 1 or 0
                us = waitForTransition(pinconf1, PIN_LOW, 80);
                if(us == -1){
                    printf("Sensor data bit [%d] timeout\n", x);
                    fflush(stdout);
                    nextState = STATE_IDLE;
                    break;
                } 

                // 0 = 26 to 28 us, which we will say is < 30 us
                // 1 = 70 us, which we will say it > 60 us
                // if high-time is between 30 us and 60 us then we don't know the bit
                if(us < 30)
                {
                    data[x] = 0;
                }
                else if(us > 60)
                {
                    data[x] = 1;
                }
                else
                {
                    printf("Sensor data bit [%d] for [%d] us is not distinguishable\n", x, us);
                    fflush(stdout);
                    nextState = STATE_IDLE;
                    break;
                }
            }

            // go back to idle state if there was an issue getting one bit
            if(x < (NUM_BITS_DATA - 1)){
                printf("Sensor data bit [%d] error. Going back to idle state\n", x, us);
                fflush(stdout);
                nextState = STATE_IDLE;
                continue;
            }

            // if we made it this far, then we have a full data[] to decode
            nextState = STATE_CALC;
        }
        //*/

        // decode the data from the sensor into RH and T
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
        //*
        if(state == STATE_CALC)
        {
            int rh = 0;
            int t = 0;
            int sum = 0;

            // convert binary to decimal for rh, t and sum
            printf("data =[");
            for(int x = 0; x < NUM_BITS_DATA; x++)
            {
                printf("%d,", data[x]);

                if(x < NUM_BITS_RH)
                {
                    rh += (data[x] << (NUM_BITS_RH - x - 1));
                }
                else if(x < NUM_BITS_T)
                {
                    t += (data[x] << (NUM_BITS_T - (x - NUM_BITS_RH) - 1));
                }
                else if(x < NUM_BITS_DATA)
                {
                    sum += (data[x] << (NUM_BITS_SUM - (x - NUM_BITS_RH - NUM_BITS_T) - 1));
                }
            }
            printf("]");

            printf("rh  = [%d]\n", rh);
            printf("t   = [%d]\n", t);
            printf("sum = [%d]\n", sum);
            fflush(stdout);

            print("Calculate checksum...");
            int sumCalc = (rh + t) & (0x00FF); // get only the lower 8 bits of the sum
            printf("sumCalc = [%d]\n", sumCalc);
            fflush(stdout);
            if(sum != sumCalc){
                printf("Incorrect check sum!");
                nextState = STATE_IDLE;
                continue;
            }

            rh = rh/CONVERSION_FACTOR;
            t = t/CONVERSION_FACTOR;

            printf("Relative Humidity = [%d]    Temperature = [%d]\n", rh, t);
            fflush(stdout);

            nextState = STATE_IDLE;
        }
        //*/

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

    return 0;
}