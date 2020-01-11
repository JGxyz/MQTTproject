#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "my_diodes.h"
#include <stdlib.h>

SPI_HandleTypeDef *hspi_ws2812b;
ws2812b_color     *ws2812b_array;
uint8_t           *buffer;

void WS2812B_Init(SPI_HandleTypeDef * spi_handler)
{
	hspi_ws2812b = spi_handler;
	ws2812b_array = calloc(LED_NUMBERS, sizeof(ws2812b_color));
	buffer = calloc((LED_NUMBERS+3) * 24, sizeof(uint8_t));
}

void WS2812B_SetDiodeRGB(int16_t diode_id, uint8_t R, uint8_t G, uint8_t B)
{
	if(diode_id >= LED_NUMBERS || diode_id < 0) return;
	ws2812b_array[diode_id].red = R;
	ws2812b_array[diode_id].green = G;
	ws2812b_array[diode_id].blue = B;
}

void three_dots_running(int color) {
	int i = 0;
	int change = 1;

	for (int c = 0; c < 20; c++) {
		for (int d = 0; d < LED_NUMBERS; d++) {
			if (d == i || d == i+change*2 || d ==i+change*4)
				set_color(color, d);
			else
				set_color(BLACK, d);
		}
		signal_diodes();
		osDelay(130);
		if (i+change*4 == LED_NUMBERS-1) {
			i = LED_NUMBERS-1;
			change = -1;
			i += change;
		} else if (i+change*4 == 0) {
			i = 0;
			change = 1;
			i += change;
		} else i+=change;
	}
}



void all_color(int data) {
	for(int i = 0; i < 10; ++i){
			 switch(data){
			 case RED: WS2812B_SetDiodeRGB(i, 255,0,0); break;
			 case BLUE: WS2812B_SetDiodeRGB(i, 0,0,255); break;
			 case GREEN: WS2812B_SetDiodeRGB(i, 0,255,0); break;
			 case WHITE: WS2812B_SetDiodeRGB(i, 255,255,255); break;
			 case MAGENTA: WS2812B_SetDiodeRGB(i, 255, 0, 255); break;
			 case CYAN: WS2812B_SetDiodeRGB(i, 0, 255, 255); break;
			 case YELLOW: WS2812B_SetDiodeRGB(i, 255, 255, 0); break;
			 default: WS2812B_SetDiodeRGB(i, 0,0,0); break;
			 }
	}
	signal_diodes();
}

void blink(int data){
	int red = 0;
	int green = 0;
	int blue = 0;
	switch(data){
			case RED: red = 255; break;
			case BLUE: blue = 255; break;
			case GREEN: green = 255; break;
			case WHITE: red = 255; green = 255; blue = 255; break;
			case MAGENTA: red = 255; blue = 255; break;
			case CYAN: green = 255; blue = 255; break;
			case YELLOW: green = 255; red = 255; break;
			default:;
	}
	for (int k=0; k<20; k++){
		for (int i = 0; i < 10; i++){
			WS2812B_SetDiodeRGB(i, red, green, blue);
		}
		signal_diodes();
		osDelay(400);
		for (int i = 0; i < 10; i++){
			WS2812B_SetDiodeRGB(i, 0, 0, 0);
		}
		signal_diodes();
		osDelay(400);
	}
}


void set_color(int data, int i) {
	 switch(data){
		case RED: WS2812B_SetDiodeRGB(i, 255,0,0); break;
		case BLUE: WS2812B_SetDiodeRGB(i, 0,0,255); break;
		case GREEN: WS2812B_SetDiodeRGB(i, 0,255,0); break;
		case WHITE: WS2812B_SetDiodeRGB(i, 255,255,255); break;
		case MAGENTA: WS2812B_SetDiodeRGB(i, 255, 0, 255); break;
		case CYAN: WS2812B_SetDiodeRGB(i, 0, 255, 255); break;
		case YELLOW: WS2812B_SetDiodeRGB(i, 255, 255, 0); break;
		default: WS2812B_SetDiodeRGB(i, 0,0,0); break;
	 }
}

void mixed(){
	all_color(BLACK);
	for (int i = 0; i < 10; i++) {
		set_color(MAGENTA, i);
		signal_diodes();
		osDelay(300);
	}
	for (int i = 0; i < 10; i++) {
		set_color(CYAN, i);
		signal_diodes();
		osDelay(300);
	}
	for (int i = 0; i < 10; i++) {
		set_color(YELLOW, i);
		signal_diodes();
		osDelay(300);
	}
}

void basic(){
	all_color(BLACK);
	for (int i = 0; i < 10; i++) {
		set_color(RED, i);
		signal_diodes();
		osDelay(300);
	}
	for (int i = 0; i < 10; i++) {
		set_color(GREEN, i);
		signal_diodes();
		osDelay(300);
	}
	for (int i = 0; i < 10; i++) {
		set_color(BLUE, i);
		signal_diodes();
		osDelay(300);
	}
}

void all(){
	all_color(BLACK);
	for (int i = 0; i < 10; i++) {
		set_color(RED, i);
		signal_diodes();
		osDelay(300);
	}
	for (int i = 0; i < 10; i++) {
		set_color(GREEN, i);
		signal_diodes();
		osDelay(300);
	}
	for (int i = 0; i < 10; i++) {
		set_color(BLUE, i);
		signal_diodes();
		osDelay(300);
	}
	for (int i = 0; i < 10; i++) {
		set_color(MAGENTA, i);
		signal_diodes();
		osDelay(300);
	}
	for (int i = 0; i < 10; i++) {
		set_color(CYAN, i);
		signal_diodes();
		osDelay(300);
	}
	for (int i = 0; i < 10; i++) {
		set_color(YELLOW, i);
		signal_diodes();
		osDelay(300);
	}
}


void change(int data){
	switch(data){
		case ALL: all(); break;
		case BASIC: basic(); break;
		case MIXED: mixed(); break;
		default:;
	}
}

void signal_diodes(){
	for(uint8_t i = 0; i < 72; i++)
		buffer[i] = 0x00;

	for(uint16_t i=0, j=72; i<LED_NUMBERS; i++)
	{

		//GREEN
		for(int8_t k=7; k>=0; k--)
		{
			if((ws2812b_array[i].green & (1<<k)) == 0)
				buffer[j] = zero;
			else
				buffer[j] = one;
			j++;
		}

		//RED
		for(int8_t k=7; k>=0; k--)
		{
			if((ws2812b_array[i].red & (1<<k)) == 0)
				buffer[j] = zero;
			else
				buffer[j] = one;
			j++;
		}

		//BLUE
		for(int8_t k=7; k>=0; k--)
		{
			if((ws2812b_array[i].blue & (1<<k)) == 0)
				buffer[j] = zero;
			else
				buffer[j] = one;
			j++;
		}

	}
	HAL_SPI_Transmit_DMA(hspi_ws2812b, buffer, (LED_NUMBERS+3) * 24);
	while(HAL_DMA_STATE_READY != HAL_DMA_GetState(hspi_ws2812b->hdmatx));
}

void all_colors() {
	for(int i = 0; i < 10; ++i){ // blue
		   	 WS2812B_SetDiodeRGB(i, 0,0,255);
		   	 signal_diodes();
		   	 osDelay(200);
	}

	for(int i = 0; i < 10; ++i){ // red
			 WS2812B_SetDiodeRGB(i, 255,0,0);
			 signal_diodes();
			 osDelay(200);
	}

	for(int i = 0; i < 10; ++i){ // green
			WS2812B_SetDiodeRGB(i, 0,255,0);
			signal_diodes();
			osDelay(200);
	}

}

void red_green() {
	for(int i = 0; i < 10; ++i){
		if (i<5)
			WS2812B_SetDiodeRGB(i, 0,255,0);
		else
			WS2812B_SetDiodeRGB(i, 255,0,0);
	}
	signal_diodes();
	osDelay(500);
	for(int i = 0; i < 10; ++i){
			if (i>=5)
				WS2812B_SetDiodeRGB(i, 0,255,0);
						else
							WS2812B_SetDiodeRGB(i, 255,0,0);
	}
	signal_diodes();
	osDelay(500);

}

void print_diode_colors(){
	for(int i = 0; i < LED_NUMBERS; ++i){
		printf("DiodNR: %d = R:%d G:%d B:%d \n",
				i,
				ws2812b_array[i].red,
				ws2812b_array[i].green,
				ws2812b_array[i].blue);
	}
}

void print_buffer(uint8_t* buffer, int length){
	printf("Buffer: (Zero = 0b11000000 = 192), (One = 0b11111000 = 248)");
	for(int i = 0; i < length; ++i){
		if(i%8==0) printf("\n");
		printf("%2d ", buffer[i]);
	}
	printf("\n");
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	HAL_SPI_DMAStop(hspi_ws2812b);
}
