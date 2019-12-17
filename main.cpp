#include <msp430fr2355.h>
#include "ws2812.h"
#include "colors.h"

//#define RAINBOW
#define BLUELINE
#define MAX_BLUE_LINES 9

uint8_t binit(uint16_t *point);
uint8_t _bcount; //number of blue lines
bool _bflag; //button-interrupt flag

int main(void)
{
    volatile uint16_t i, wdr;
    volatile uint8_t bcount;
    bool bcolor;
    static uint16_t point[10];

    WDTCTL = WDTPW | WDTHOLD;    // Stop WDT
    PM5CTL0 &= ~LOCKLPM5;       //disable LOW POWER MODE
    CSCTL1_L |= 0x0A;           //set to 8Mhz ????


    strip.init(); //init the LED-Strip, set to black


    i = 0;
    wdr = 0;

#ifdef RAINBOW
    Rainbow();
#endif
#ifdef BLUELINE

    //interrupt buddon 1
    P4OUT = 0;
    P4DIR = 0;
    P4IE = 0;
    CLR_BIT(P4SEL0,1);
    SET_BIT(P4REN,1);
    SET_BIT(P4OUT,1);
    CLR_BIT(P4IFG,1);
    SET_BIT(P4IES,1);
    SET_BIT(P4IE,1);

    //interrupt buddon 2
    P2OUT = 0;
    P2DIR = 0;
    P2IE = 0;
    CLR_BIT(P2SEL0,3);
    SET_BIT(P2REN,3);
    SET_BIT(P2OUT,3);
    CLR_BIT(P2IFG,3);
    SET_BIT(P2IES,3);
    SET_BIT(P2IE,3);



    _bcount = 5;

    bcount = binit(point);

while(1)
{
    SET_BIT(P6OUT,6);

    bcolor = true;
    for(i = bcount; i; i--)
    {

        strip.line(point[i]);
        bcolor = !bcolor;
        point[i]--;
        if(point[i] == 0)
            point[i] = NUM_LEDS+BLUE_STRIP_LEN;
    }

    strip.show();
    CLR_BIT(P6OUT,6);
    wait_ms(100);
    wdr++;

    if(wdr > 0xFF)
    {
        SET_BIT(P4DIR,0);
        SET_BIT(P4OUT,0);
    }


    if(_bflag)
    {
        bcount = binit(point);
        strip.clear();
    }
}

}

// Port 4 interrupt service routine
// button press -> increment bcount
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    P4IFG = 0; //reset flag / clear every interrupt
    if(_bcount < MAX_BLUE_LINES)
    {
        _bcount++;
        _bflag = 1;
    }
}
// Port 2 interrupt service routine
// button press -> decrement bcount
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    P2IFG = 0; //reset flag / clear every interrupt
    if(_bcount)
    {
        _bcount--;
        _bflag = 1;
    }
}

uint8_t binit(uint16_t *point)
{
    uint16_t i, div;
    _bflag = 0;
    div = NUM_LEDS / _bcount;
    for(i=_bcount; i; i--)
    {
        point[i] = i*div;
    }
    return _bcount;
}
#endif
