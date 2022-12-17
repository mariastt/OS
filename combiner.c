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
pthread_mutex_t mutex_stepmot= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_keypad= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_gp2y = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_signal_exit = PTHREAD_MUTEX_INITIALIZER;

bool keypad_perm;
bool stepmot_perm;
bool signal_exit;
char angle[100];



static void *thread_console_check()
{
    char signalch_exit = 's';
    int command = 0;
    int fd;
    int ret;

    
    pid_t pid_stepmot;
    pid_t pid_gp2y;

    while (1) {
        sleep(1);
        printf("Stop stepmotor - 1\n");
        printf("Stop range - 2\n");
        printf("Stop all - 3\n");
        printf("Staart  stepmotor - 4\n");
        printf("Staart range - 5\n");
        printf("Set angle - 6\n");
        printf("Введите команду : ");
        
        scanf("%d", &command);

        switch(command) {
            case 1:
                // передача в signal_lcd сигнал окончания программы
                fd = open("step_data", O_WRONLY);
                if (fd == -1) {
                     exit(-1);

                }

                write(fd, (void *)&signalch_exit, sizeof(signalch_exit));
                printf("Stop stepmotor! \n");
                close(fd);
                break;
            case 2:
                // передача в signal_encoder сигнал окончания программы
                fd = open("gp2y_data", O_WRONLY);
                if (fd == -1) {
                     exit(-1);
                }
                write(fd, (void *)&signalch_exit, sizeof(signalch_exit));
                printf("Stop gp2y! \n");
                close(fd);
                break;
            case 3:
                fd = open("step_data", O_WRONLY);
                if (fd == -1) {
                     exit(-1);
                }

                write(fd, (void *)&signalch_exit, sizeof(signalch_exit));
                printf("Stop stepmotor! \n");
                close(fd);
                
                fd = open("gp2y_data", O_WRONLY);
                if (fd == -1) {
                     exit(-1);
                }
                write(fd, (void *)&signalch_exit, sizeof(signalch_exit));
                printf("Stop gp2y! \n");
                close(fd);

                break;
            case 4:
         
                pid_stepmot = fork();
                static char *argv_path[] = {"step_motor", "-q", "1" };
                if (!pid_stepmot) {
                    execv("/home/pi/IVT_32_Stepanova/kr6/step_motor", argv_path);
                }
                break;
            case 5:
        
                pid_gp2y = fork();
                static char *argv_gp2y[] = {"rangefinder_gp2y", "-q", "1000"};
                if (!pid_gp2y) {
                    execv("/home/pi/IVT_32_Stepanova/kr6/rangefinder_gp2y", argv_gp2y);
                }
                break;
            case 6:
                printf("введите угол: ");
                scanf("%s", &angle);
                stepmot_perm = true;
                keypad_perm = true;

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
        exit(-1);



    while(1) {
       
        pthread_mutex_lock(&mutex_stepmot);
        if(stepmot_perm){
            write(stepmot,angle,15);
            stepmot_perm = false;
            keypad_perm = false;
        }

        pthread_mutex_unlock(&mutex_stepmot);
    }
}

// функция обработки кнопок
// текущий gpio кнопки 26
static void *keypad(void *argv)
{
   int keypd = open("keypad_data", O_RDWR);
	if (keypd  == -1)
       exit(-1);


    
        
    
    char keystr[3];
      
    char buffer1[16];
    char *sym = (char*)malloc(1);

while(1)
{
	read(keypd, buffer1, sizeof(buffer1));
	*sym = buffer1[0];
    
    pthread_mutex_lock(&mutex_keypad);  
        if  ( !keypad_perm ){

        
 		if(buffer1[0]== '*')keypad_perm = true;
            
        else {
            strcat(angle, sym);
        //printf("%d\n",atoi(str));
        //printf("%d\n",atoi(angle)); 
        }
        if(atoi(angle)%1000== 647)
        *angle = 0;
        
        }
        pthread_mutex_unlock(&mutex_keypad);

        
    
}
}

// функция обработки именнованнымии каналов
static void *gp2y(void *argv)
{
    int range = open("gp2y_data", O_RDWR);
	if (range  == -1)
        exit(-1);

    while(1) {
        
        char buffer2[16];
        read(range, buffer2, sizeof(buffer2));
        pthread_mutex_lock(&mutex_gp2y);
        if(keypad_perm){
            //printf("here2\n");
            

            if(buffer2[0]== 'o'){
            stepmot_perm = true;
            }
        }   
        pthread_mutex_unlock(&mutex_gp2y);
    }
}

int main(int argc, char **argv) 
{
    
    *angle='0';
    signal_exit = false;
    
    
    pthread_t thread_gp2y; 
    pthread_t thread_keypad;
    pthread_t thread_stepmot;
    pthread_t thread_console; // поток для считывания с консоли

   // создание потоков
    pthread_create(&thread_gp2y, NULL, gp2y, NULL);
    pthread_create(&thread_keypad, NULL, keypad, (void *)argv[1]);
    pthread_create(&thread_stepmot, NULL, stepmot, NULL);
    pthread_create(&thread_console, NULL, thread_console_check, NULL);

    pthread_join(thread_gp2y, NULL);
    pthread_join(thread_keypad, NULL);
    pthread_join(thread_stepmot, NULL);
    pthread_join(thread_console, NULL);
    
    return 0;
}
