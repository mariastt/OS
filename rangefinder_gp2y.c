/*******************************************************************************
 * Copyright (c) 2022 Gerasimenko Evgeniy (evgenyger.work@gmail.com)                      *
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
#include <stdbool.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <ads1115.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <pthread.h>





#define AD_BASE 122
#define AD_ADDR 0x48 //i2c address

#define ADC_PIN 2

pthread_mutex_t mutex_signal_exit = PTHREAD_MUTEX_INITIALIZER;
bool signal_exit;
int quiet;
int delay_ms;

void help()
{
	printf("    Use this application for reading from rangefinder\n");
	printf("    execute format: ./range TIME \n");
	printf("    return: length in cm\n");
	printf("    TIME - pause between writing in ms\n");
	printf("    -h - help\n");
	printf("    -q - quiet\n");
}

int clamp(int x, int min, int max)
{
	return (x < min) ? min : ((x > max) ? max : x);
}

static void *signal_catch()
{
	int range_read = open("gp2y_data", O_RDONLY);
		if (range_read  == -1)
			exit(-1);
	char signalch = '0';
	
	while(1){
 	read(range_read, (void *)&signalch, sizeof(char));
	
	pthread_mutex_lock(&mutex_signal_exit);
	if(signalch=='s') {
		signal_exit = true;
		printf("Signal exit !\n");
		exit(0);
		pthread_exit(NULL);
	}
	pthread_mutex_unlock(&mutex_signal_exit);
	}
	close(range_read);
	
}


static void *write_data()
{

int range_write = open("gp2y_data", O_WRONLY);
	if (range_write  == -1)
		exit(-1);


	

	wiringPiSetup();
	ads1115Setup(AD_BASE, AD_ADDR);
	digitalWrite(AD_BASE, 0);

	while (1) {
		int ADC_VAL = analogRead(AD_BASE + ADC_PIN);
		if (!quiet)
			printf("ADC: %d \n", ADC_VAL);
		else{
			//double rez = pow(   (61.3899*(1000/(ADC_VAL*0.1875))), 1.1076);
			//printf("%f\n", rez);
			if(pow(   (61.3899*(1000/(ADC_VAL*0.1875))), 1.1076) < 100 && pow(   (61.3899*(1000/(ADC_VAL*0.1875))), 1.1076) > 30 ){
			write(range_write,"ok\n",3);
			//printf("ok\n");
			}//else write(range,"no\n",3);
		}
		fflush(stdout);  // сброс буфера stdout
		usleep(1000 * delay_ms);
	}
	

}

int main(int argc, char *argv[])
{
	
	bool signal_exit = false;
	quiet = 0;
	if (argc > 1) {
		if ((strcmp(argv[1], "-h") == 0)) {
			help();
			return 0;
		} else {
			if ((strcmp(argv[1], "-q") == 0)) {
				quiet = 1;
			}
		}
	}

	if ((quiet && argc != 3) || (!quiet && argc != 2)) {
		help();
		return 0;
	}

	if (!quiet)
		printf("\nThe rangefinder application was started\n\n");

	int argument = 1;
	if (quiet)
		argument++;
	delay_ms = atoi(argv[argument]);
	pthread_t thread_signal_catch; 
    pthread_t thread_write_data;
    

    
    pthread_create(&thread_signal_catch, NULL, signal_catch, NULL);
    pthread_create(&thread_write_data, NULL, write_data, NULL);

	pthread_join(thread_signal_catch, NULL);
    if (signal_exit) {
        printf("Program close!\n");
        return 0;
    }
	pthread_join(thread_write_data, NULL);
	return 0;
}
