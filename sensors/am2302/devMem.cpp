#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>

#define OE_ADDR 0x134
#define GPIO_DATAOUT 0x13C
#define GPIO_DATAIN 0x138
#define GPIO0_ADDR 0x44E07000
#define GPIO1_ADDR 0x4804C000
#define GPIO2_ADDR 0x481AC000
#define GPIO3_ADDR 0x481AF000

#define GPIO_PIN_NUMBER (31) // ex. if we are in GPIO0, then 30 here would mean GPIO0_30, where as GPIO1 would be GPIO1_30 

using namespace std;

int main()
{
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    ulong* pinconf1 =  (ulong*) mmap(NULL, 0x1000, PROT_READ, MAP_SHARED, fd, GPIO0_ADDR);

    for(int i = 0; i < 250; i++)
    {
        //cout << pinconf1[GPIO_DATAIN/4]  << endl;

        // get pin GPIO_PIN_NUMBER in GPIOX
        if(pinconf1[GPIO_DATAIN/4] & (1  << GPIO_PIN_NUMBER))
        {
            cout << "high" <<endl;
        }
        else
        {
            cout << "low" <<endl;
        }

        usleep(500000);
    }

    return 0;
}