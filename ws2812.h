
#ifndef WS2812_h
#define WS2812_h
#include <msp430.h>
#include "stdint.h"
#include "port_bit.h"

// Configuration - SET THESE!
/**************************************/
#define NUM_LEDS    70
#define BRIGHTNESS 0xFF
//#define UPDint //TODO
/**************************************/

// Transmit codes
#define HIGH_CODE   (0xF0)      // b11110000
#define LOW_CODE    (0xC0)      // b11000000

#define HIGH_CODE_f   (0xF8)      // b11111000
#define LOW_CODE_f   (0xE0)      // b11100000

#define TTO(to,from) to.red = from.red; to.green = from.green; to.blue = from.blue;

#define BLUE_STRIP_LEN 16
static uint8_t BLUE_STRIP[BLUE_STRIP_LEN] = {10,25,50,100,150,200,250,250,200,150,150,100,50,25,10,0};

// WS2812 takes GRB format
typedef struct {
    uint8_t green;
    uint8_t red;
    uint8_t blue;
} LED;

class LEDStrip
{
public:
    enum COLOR
    {
        BLACK,RED,GREEN,BLUE,CYAN,PURPLE,YELLOW,WHITE
    };

    // config strip
    void init();

    // sends data to strip, updates them
    void show();

    // Set the color of a certain LED
    void setColor(uint16_t p, uint8_t r, uint8_t g, uint8_t b);
    void setColor(uint16_t p, enum COLOR color);
    //void setColor(uint16_t p, LED color);
    void line(uint16_t i);

    // Clear the color of all LEDs (make them black/off)
    void clear(void);

    void shift();

    // Fill the strip with a solid color.
    void fill(uint8_t r, uint8_t g, uint8_t b);
    void fill(enum COLOR color);

    uint16_t int_id; //led ID
    uint16_t int_color; //led color
    volatile uint8_t int_bit; //bit of led color
    uint8_t *int_rgb; //pointer

private:
    LED leds[NUM_LEDS];
    //


} strip;

#endif

