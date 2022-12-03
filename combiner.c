#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <ads1115.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

 

int main(){
int keypd = open("keypad_data", O_RDWR);
	if (keypd  == -1)
        return -1;
	
	int range = open("gp2y_data", O_RDWR);
	if (range  == -1)
        return -1;

int stepmot = open("step_data", O_RDWR);
	if (stepmot  == -1)
        return -1;
char keystr[3];
char str[100];
//str[0]="0";    
char buffer1[16];
char *sym = (char*)malloc(1);

while(1)
{
	read(keypd, buffer1, sizeof(buffer1));
	*sym = buffer1[9];
    if(buffer1[0]== 'p'){ 
	
 		if(buffer1[9]== '*')
        {
            printf("pressed *: ");
            while(1){
                char buffer2[16];
            	read(range, buffer2, sizeof(buffer2));

                if(buffer2[0]== 'o'){
                printf(":расстояние: (разрешено)");
                
                write(stepmot,str,15);
                break;
                }

            }
            
		}else strcat(str, sym);
        
        
        printf("%d\n",atoi(str)%1000);
        if(atoi(str)%1000== 647)
        *str = 0;
        }

}






}