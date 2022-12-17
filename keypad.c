/*******************************************************************************
 * Copyright (c) 2022 Sergey Balabaev (sergei.a.balabaev@gmail.com)                     *
 *                                                                             *
 * The MIT License (MIT):                                                      *
 * Permission is hereby granted, free of charge, to any person obtaining a     *
 * copy of this software and associated documentation files (the "Software"),  *
 * to deal in the Software without restriction, including without limitation   *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell   *
 * copies of the Software, and to permit persons to whom the Software is       *
 * furnished to do so, subject to the following conditions:                    *
 * The above copyright notice and this permission notice shall be included     *
 * in all copies or substantial portions of the Software.                      *
 *                                                                             *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,             *
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR       *
 * OTHER DEALINGS IN THE SOFTWARE.                                             *
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bcm2835.h>
#define ROWS 4
#define COLS 3

char pressedKey = '\0';
#ifdef VAR6
int rowPins[ROWS] = { 18, 17, 16, 25 }; // R0, R1, R2, R3
int colPins[COLS] = { 20, 21, 19 }; // C0, C1, C2
#endif

#ifdef VAR8
int rowPins[ROWS] = { 4, 25, 11, 8 }; // R0, R1, R2, R3
int colPins[COLS] = { 7, 6, 5 }; // C0, C1, C2
#endif

#ifdef VAR10
int rowPins[ROWS] = { 18, 17, 16, 25 }; // R0, R1, R2, R3
int colPins[COLS] = { 20, 21, 19 }; // C0, C1, C2
#endif


char keys[ROWS][COLS] = { { '1', '2', '3' },
			  { '4', '5', '6' },
			  { '7', '8', '9' },
			  { '*', '0', '#' } };

void init_keypad()
{
	for (int c = 0; c < COLS; c++) {
		bcm2835_gpio_fsel(colPins[c], BCM2835_GPIO_FSEL_INPT);
		bcm2835_gpio_write(colPins[c], LOW);
	}

	for (int r = 0; r < ROWS; r++) {
		bcm2835_gpio_fsel(rowPins[r], BCM2835_GPIO_FSEL_OUTP);
		bcm2835_gpio_write(rowPins[r], HIGH);
	}
}

int findHighCol()
{
	for (int c = 0; c < COLS; c++) {
		if (bcm2835_gpio_lev(colPins[c]) == 1)
			return c;
	}
	return -1;
}

char get_key()
{
	int colIndex;
	for (int r = 0; r < ROWS; r++) {
		bcm2835_gpio_write(rowPins[r], HIGH);
		colIndex = findHighCol();
		if (colIndex > -1) {
			if (!pressedKey)
				pressedKey = keys[r][colIndex];
			return pressedKey;
		}
		bcm2835_gpio_write(rowPins[r], LOW);
	}
	pressedKey = '\0';
	return pressedKey;
}

void help()
{
	printf("    Use this application for keypad\n");
	printf("    execute format: ./keypad [-h] \n");
	printf("    return: enter button\n");
	printf("    -h - help\n");
	printf("    -q - quiet\n");
}

int main(int argc, char *argv[])
{
	int quiet = 0;
	if (argc > 1) {
		if ((strcmp(argv[1], "-h") == 0)) {
			help();
			return 0;
		} else if ((strcmp(argv[1], "-q") == 0)) {
			quiet = 1;
		} else {
			help();
			return 0;
		}
	}
	bcm2835_init();
	init_keypad();
	if (!quiet)
		system("clear");
	while (1) {
		char x = get_key();
		if (x) {
			if (!quiet) {
				system("clear");
				printf("pressed: %c\n", x);
			} else
				printf("%c\n", x);
		} else if (!quiet) {
			system("clear");
			printf("no key pressed\n");
		}
		bcm2835_delay(500);
		fflush(stdout);
	}
	bcm2835_close();
	return 0;
}
