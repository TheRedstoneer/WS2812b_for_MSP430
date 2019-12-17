#include "ws2812.h"

#ifdef UPDint
#pragma vector = USCI_A0_VECTOR
__interrupt void data_sent(void)
{
    TOGGLE_BIT(P6OUT,4);
    TOGGLE_BIT(P6OUT,6);
    UCA0IFG = 0;

    //strip.int_bit++;
    __no_operation();

 //called after last sent, so that there will be no endless loop...
if(strip.int_bit > 40)
{
    strip.int_bit = 0;
    __no_operation();
}

else
{

       //USCI_SPI_UCTXIFG

    strip.updateColorAsInt();
    if (TEST_BIT(BRIGHTNESS,strip.int_bit) && TEST_BIT(strip.int_rgb[strip.int_color],strip.int_bit))
        UCA0TXBUF = HIGH_CODE;
    else
        UCA0TXBUF = LOW_CODE;   // send 0

    strip.int_bit++;
    if(strip.int_bit > 7) //reached last bit
    {
        strip.int_bit = 0;
        strip.int_color++;
        if(strip.int_color > 2) //reached last color
        {
            TOGGLE_BIT(P1OUT,0);
            strip.int_color = 0;
            strip.int_id++;
            if(strip.int_id >= NUM_LEDS) //end of strip
            {
                strip.int_bit = 42;
                strip.int_color = strip.int_id = 0;
            }
        }
    }
}
__delay_cycles(2000);
}
#endif


// Initializes everything needed to use this library.
void LEDStrip::init()
{
    // configure output pin as SPI output
    SET_BIT(P1SEL0,7);

    UCA0CTLW0 |= UCSWRST;                     // **Put state machine in reset**
    UCA0CTLW0 |= UCMST|UCSYNC|UCCKPL|UCMSB;   // 3-pin, 8-bit SPI master
                                              // Clock polarity high, MSB
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0BRW_L = 0x01;
    UCA0BRW_H = 0x0;
    UCA0MCTLW = 0;                            // No modulation
    UCA0CTLW0 &= ~UCSWRST;
    //UCA0IE |= UCTXIE; //enable TX interrupt
    __bis_SR_register(GIE);
    clear();           // clear the strip
}

// Sets the color of a certain LED
void LEDStrip::setColor(uint16_t p, uint8_t r, uint8_t g, uint8_t b) {
    leds[p].green = g;
    leds[p].red = r;
    leds[p].blue = b;
}

void LEDStrip::setColor(uint16_t p, COLOR color)
{
    switch(color)
    {
    case RED:
        setColor(p,255,0,0);
        break;
    case GREEN:
        setColor(p,0,255,0);
        break;
    case BLUE:
        setColor(p,0,0,255);
        break;
    case CYAN:
        setColor(p,0,255,255);
        break;
    case PURPLE:
        setColor(p,255,0,255);
        break;
    case YELLOW:
        setColor(p,255,40,0);
        break;
    case WHITE:
        setColor(p,255,255,200);
        break;
    default:
        setColor(p,0,0,0);
    }
}

// Send colors to the strip and show them. Disables interrupts while processing.
void LEDStrip::show()
{

#ifdef UPDint
    int_id = int_color = int_bit = 0;
    data_sent();
#else
    // send RGB color for every LED
    volatile uint16_t i, j;
    for (i = 0; i < NUM_LEDS; i++)
    {
        //updateColorAsInt();
        uint8_t *rgb = (uint8_t *)&leds[i]; // get GRB color for this LED
        uint8_t x;
        // send green, then red, then blue
        for (j = 0; j < 3; j++)
        {
            for(x = 8; x > 0; x--)
            {
                //TODO direkt byte innen buffer schreiben
                //while (!UCA0IFG);    // wait to transmit
                if (TEST_BIT(BRIGHTNESS,x-1) && TEST_BIT(rgb[j],x-1))
                    UCA0TXBUF = HIGH_CODE_f;
                else
                    UCA0TXBUF = LOW_CODE_f;   // send 0
            }
        }
    }

     //send RES code for at least 50 us (800 cycles at 16 MHz)

#endif
}

void LEDStrip::shift()
{
    uint16_t i;

    //LED end_temp = leds[NUM_LEDS-1];



    for(i = NUM_LEDS-1; i > 0; i--)
    {
        TTO(leds[i],leds[i-1])
    }

    //TTO(leds[0], end_temp);
    setColor(0, BLACK);
    __no_operation();
    //setColor()
}

/* creates a blue line in the strip (9 long)
 * like this (in %)
 * 0 10 50 75 100 100 100 75 50 10
 * does NOT refresh the strip!
 */
void LEDStrip::line(uint16_t i)
{
    uint8_t x;
    int tmp;
    for(x = BLUE_STRIP_LEN; x; x--)
    {
        tmp = i - x;
        // test for begin/end!
        if(tmp < 0) continue;
        if(tmp >= NUM_LEDS) break;

        setColor(tmp, 0,0,BLUE_STRIP[(uint8_t)(x-1)]);

    }
}

// Clear the color of all LEDs (make them black/off)
void LEDStrip::clear()
{
    fill(BLACK);
    show();  // refresh strip
}

// Fill the strip with a solid color. This will update the strip.
void LEDStrip::fill(COLOR color)
{
    uint16_t i;
        for (i = NUM_LEDS; i > 0; i--)
            setColor(i-1, color);  // set all LEDs to specified color
}

void LEDStrip::fill(uint8_t r, uint8_t g, uint8_t b)
{
    uint16_t i;
    for (i = NUM_LEDS; i > 0; i--)
        setColor(i-1, r, g, b);  // set all LEDs to specified color
}
