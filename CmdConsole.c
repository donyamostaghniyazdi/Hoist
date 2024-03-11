#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define SZ 1000
int DescriptionXFile, DescriptionZFile, WatchdogPointer;

void ShowCmd()
{
    system("clear");
    printf("COMMANDS: \n");
    printf("T : UP\n");
    printf("G : DOWN\n");
    printf("F : LEFT\n");
    printf("H : RIGHT\n");
    printf("Z : Stop Z movement\n");
    printf("X : Stop X movement\n\n");
    fflush(stdout);
}

int main(int argc, char *argv[])
{
	char Buff[SZ];
	char CharInput[100];
    char *MotorXFifo = "/tmp/CMDMotorX";
	mkfifo(MotorXFifo, 0666);
    char *MotorZFifo = "/tmp/CMDMotorZ";
	mkfifo(MotorZFifo, 0666);
    char *WatchdogFifo = "/tmp/pWatchdog";
	mkfifo(WatchdogFifo, 0666);
    char *CmdFifo = "/tmp/pCommand";
	mkfifo(CmdFifo, 0666);
    int FileDescription = open(WatchdogFifo, O_RDONLY);
    read(FileDescription, Buff, SZ);
    WatchdogPointer = atoi(Buff);
    close(FileDescription);
    int FileCommand = open(CmdFifo, O_WRONLY);
    sprintf(Buff, "%d", (int)getpid());
    write(FileCommand, Buff, SZ);
    close(FileCommand);    
    DescriptionXFile = open(MotorXFifo, O_WRONLY);
    DescriptionZFile = open(MotorZFifo, O_WRONLY);
    while (1)
    {
		ShowCmd();
        scanf("%s", CharInput);        
        if (strlen(CharInput) > 1)
        {
            printf("Wront Input!\n");
            fflush(stdout);
        }
        else
        {
            char InputKey[10];
            sprintf(InputKey, "%d", CharInput[0]);
            switch (CharInput[0])
            {
            case 'T': 
			case 't':
                printf("Motor Z : Up\n");
                fflush(stdout);
                write(DescriptionZFile, InputKey, strlen(InputKey) + 1);
                kill(WatchdogPointer, SIGUSR1);
                break;
            case 'G': 
			case 'g':
                printf("Motor Z : Down\n");
                fflush(stdout);
                write(DescriptionZFile, InputKey, strlen(InputKey) + 1);
                kill(WatchdogPointer, SIGUSR1);
                break;
            case 'F': 
			case 'f':
                printf("Motor X : Left\n");
                fflush(stdout);
                write(DescriptionXFile, InputKey, strlen(InputKey) + 1);
                kill(WatchdogPointer, SIGUSR1);
                break;
            case 'H': 
			case 'h':
                printf("Motor X : Right\n");
                fflush(stdout);
                write(DescriptionXFile, InputKey, strlen(InputKey) + 1);
                kill(WatchdogPointer, SIGUSR1);
                break;
            case 'Z': 
			case 'z':
                printf("Z : Stop\n");
                fflush(stdout);
                write(DescriptionZFile, InputKey, strlen(InputKey) + 1);
                kill(WatchdogPointer, SIGUSR1);
                break;
            case 'X': 
			case 'x':
                printf("X : Stop\n");
                fflush(stdout);
                write(DescriptionXFile, InputKey, strlen(InputKey) + 1);
                kill(WatchdogPointer, SIGUSR1);
                break;
            default:
                printf("-\n");
                fflush(stdout);
                break;
            }
        }
    }
    close(DescriptionXFile);    
    close(DescriptionZFile);
	unlink(MotorXFifo);
    unlink(MotorZFifo);
    return 0;
}
