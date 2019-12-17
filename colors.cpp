/*
 * Some nice colors
 *
 *  Created on: 15.08.2019
 *      Author: David
 */
#include "colors.h"

void Rainbow()
{
  uint16_t i;
  uint8_t j = 0, x;

  while(1)
  {
    for(i=NUM_LEDS; i>0; i--)
    {
        x = (uint8_t) i+j;
        if(x < 85)
        {
            strip.setColor(i, x * 3, 255 - x * 3, 0);
        }
        else if(x < 170)
        {
            x -= 85;
            strip.setColor(i, 255 - x * 3, 0, x * 3);
        }
        else
        {
            x -= 170;
            strip.setColor(i, 0, x * 3, 255 - x * 3);
        }
    }

    j++; //can overflow, to start back at 0
    strip.show();
    wait_ms(100);
  }
}

/**
 *
 */
void wait_ms(uint16_t ticks)
{
    if (!ticks || (ticks > 1800)) return;
    TB1CCTL0 |= CCIE;                   // TBCCR0 interrupt enabled

    TB1CCR0 = 33 * ticks;              // 1ms
    TB1CTL = TBCLR;                             // Clear Timer
    TB1CTL = TBSSEL__ACLK | MC__UP;     // Input divider /4, ACLK, UP mode
    __bis_SR_register(LPM3_bits | GIE);         // Enter LPM3 w/ interrupt

}

