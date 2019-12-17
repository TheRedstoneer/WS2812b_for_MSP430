
#define SET_BIT(x,y) (x|=(1<<y))
#define CLR_BIT(x,y) (x&=(~(1<<y)))
#define TEST_BIT(x,y) (x & (1<<y))
#define TOGGLE_BIT(x,y) (x^=(1<<y))
#define HIGH_NIBBLE(x) (x >> 4)
