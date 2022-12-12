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
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
 
pthread_mutex_t mutex_angle = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_stepmot_perm = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_keypad_perm = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_gp2y = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_signal_exit = PTHREAD_MUTEX_INITIALIZER;

bool keypad_perm;
bool stepmot_perm;
bool signal_exit;
char angle[100];

static void *thread_console_check()
{
    char signalch_exit = '1';
    int command = 0;
    int fd;
    int ret;

    pid_t pid_encoder;
    pid_t pid_display;

    while (1) {
        sleep(1);
        printf("Stop display - 1\n");
        printf("Stop encoder - 2\n");
        printf("Stop all - 3\n");
        printf("Run display - 4\n");
        printf("Run encoder - 5\n");
        printf("Введите команду : ");
        scanf("%d", &command);

        switch(command) {
            case 1:
                // передача в signal_lcd сигнал окончания программы
                fd = open("signal_lcd", O_WRONLY);
                if (fd == -1) {
                    printf("Failed open to signal_lcd !\n");
                }

                ret = write(fd, (void *)&signalch_exit, sizeof(signalch_exit));
                printf("Stop display! \n");
                close(fd);
                break;
            case 2:
                // передача в signal_encoder сигнал окончания программы
                fd = open("signal_encoder", O_WRONLY);
                if (fd == -1) {
                    printf("Failed open to signal_encoder", O_WRONLY);
                }
                write(fd, (void *)&signalch_exit, sizeof(signalch_exit));
                printf("Stop encoder! \n");
                close(fd);
                break;
            case 3:
                fd = open("signal_lcd", O_WRONLY);
                if (fd == -1) {
                    printf("Failed open to signal_lcd !\n");
                }

                ret = write(fd, (void *)&signalch_exit, sizeof(signalch_exit));
                printf("Stop display! \n");
                close(fd);

                fd = open("signal_encoder", O_WRONLY);
                if (fd == -1) {
                    printf("Failed open to signal_encoder", O_WRONLY);
                }
                write(fd, (void *)&signalch_exit, sizeof(signalch_exit));
                printf("Stop encoder! \n");
                close(fd);
                break;
            case 4:
            // нужен фикс для всех запусков
                pid_display = fork();
                //передать в аргумент путь до lcd_fifo
                ///home/pi/../encoder_fifo
                static char *argv_display[] = {"lcd", "../encoder/encoder_fifo"};
                if (pid_display) {
                    execv("../lcd/lcd", argv_display);
                }
                break;
        }
        // } else if (command == "start display") {
            // pid_display = fork();
            // передать в аргумент путь до lcd_fifo
            // /home/pi/../encoder_fifo
            // static char *argv_display[] = {"lcd", "/home/pi/../encoder_fifo"};
            // if (pid_display) {
            //     execv("../lcd/lcd", argv_display);
            // }

        // } else if (command == "start encoder") {
        //     // pid_encoder = fork();
        //     // static char argv_encoder[] = {"../encoder/encoder", "-q"};
        //     // if (pid_encoder) {
        //     //     execv("../encoder/encoder", argv_encoder);
        //     // }
ы
        // } else if (command == "set time") {
        //     printf("Enter time :");
        //     pthread_mutex_lock(&mutex_time);
        //     scanf("%d\n", &_time);
        //     printf("Установленно новое время %ld \n", _time);
        //     pthread_mutex_unlock(&mutex_time);
        // } else {
        //     printf("Undefined command \n");
        // }
    }
}

// т.к время может быть не "настоящим", а результатом изменений, то сделаем такой костыль
static void *stepmot() 
{

    int stepmot = open("step_data", O_RDWR);
	if (stepmot  == -1)
        return -1;


    while(1) {
       
        pthread_mutex_lock(&mutex_stpepmotor);
        if(stepmot_perm){
            write(stepmot,str,15);
            stepmot_perm = false;
            keypad_perm = false;
        }

        pthread_mutex_unlock(&mutex_stpepmotor);
    }
}
// функция обработки кнопок
// текущий gpio кнопки 26
static void *keypad(void *argv)
{
   int keypd = open("keypad_data", O_RDWR);
	if (keypd  == -1)
        return -1;

    
        
    
    char keystr[3];
      
    char buffer1[16];
    char *sym = (char*)malloc(1);

while(1)
{
	read(keypd, buffer1, sizeof(buffer1));
	*sym = buffer1[0];
    char buffer[16];
    pthread_mutex_lock(&mutex_angle);    
 		if(buffer1[0]== '*')keypad_perm = true;    
        else strcat(str, sym);
        //printf("%d\n",atoi(str));
        if(atoi(str)%1000== 647)
        *str = 0;
        

        pthread_mutex_unlock(&mutex_angle);

        
    
}
}


// функция обработки именнованнымии каналов
static void *gp2y(void *argv)
{
    int range = open("gp2y_data", O_RDWR);
	if (range  == -1)
        return -1;

    while(1) {
        if(keypad_perm){
            char buffer2[16];
            read(range, buffer2, sizeof(buffer2));

            if(buffer2[0]== 'o'){
            stepmot_perm = true;
            }
        }   
    }
}

int main(){

	



char keystr[3];
char str[100];
//str[0]="0";    
char buffer1[16];
char *sym = (char*)malloc(1);

while(1)
{
	read(keypd, buffer1, sizeof(buffer1));
	*sym = buffer1[0];
    char buffer[16];
        
 		if(buffer1[0]== '*')
        {
            //printf("pressed *: ");
            while(1){
                char buffer2[16];
            	read(range, buffer2, sizeof(buffer2));

                if(buffer2[0]== 'o'){
                time_t rawtime;
                struct tm * timeinfo;

                time ( &rawtime );
                timeinfo = localtime ( &rawtime );
                printf("время изменения положения двигателя: %s", asctime (timeinfo));
                printf("угол установлен на: %d\n",atoi(str)%1000);
                write(stepmot,str,15);
		        //read(stepmot, buffer, sizeof(buffer));
               // printf("время время окончания движения: %s", asctime (timeinfo));

                break;
                }

            }
            
		}else strcat(str, sym);
        
        
        //printf("%d\n",atoi(str));
        if(atoi(str)%1000== 647)
        *str = 0;
        

}






}
int main(int argc, char **argv) 
{
    

    signal_exit = false;
    button_record = false;
    _time = time(NULL);
    
    pthread_t thread_gp2y; 
    pthread_t thread_keypad;
    pthread_t thread_stepmot;
    pthread_t thread_console; // поток для считывания с консоли

    pthread_create(&thread_gp2y, NULL, button_handler, NULL);
    pthread_create(&thread_keypad, NULL, pipe_handler, (void *)argv[1]);
    pthread_create(&thread_stepmot, NULL, time_counter, NULL);
    pthread_create(&thread_console, NULL, thread_console_check, NULL);

    pthread_join(thread_console, NULL);
    if (signal_exit) {
        printf("Program close!\n");
        exit(0);
    }
    pthread_join(thread_button_handler, NULL);
    pthread_join(thread_pipe_handler, NULL);
    pthread_join(thread_time, NULL);
    
    return 0;
}
