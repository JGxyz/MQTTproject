#ifndef MY_DIODES_H_
#define MY_DIODES_H_

#define zero 0b11000000
#define one 0b11111000
#define LED_NUMBERS 10

enum Color {
	RED, GREEN, BLUE, WHITE, BLACK, MAGENTA, CYAN, YELLOW
};

enum Topic {
	COLOR, BLINK, CHANGE, NONE
};

enum Change {
	ALL, MIXED, BASIC
};

typedef struct ws2812b_color {
	uint8_t red, green, blue;
} ws2812b_color;

void my_own_sender();
void WS2812B_Init(SPI_HandleTypeDef * spi_handler);
void WS2812B_SetDiodeRGB(int16_t diode_id, uint8_t R, uint8_t G, uint8_t B);
void print_diode_colors();
void print_buffer(uint8_t* buffer, int length);
void all_color(int);
void blink();
void all_colors();
void red_green();
void change(int);
void set_color(int,int);
void all();
void basic();
void mixed();
void three_dots_running(int);

#endif
