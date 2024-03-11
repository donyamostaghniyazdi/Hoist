#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>

#define SZ 1000
int FileDescX, FileDescZ, FileDescVX, FileDescVZ, WatchdogPointer;
int MotorXProccessID,MotorZProccessID,FileDescriptionInput, InspFileDescription;
float POSX = 0, POSZ = 0;
void Show()
{
    system("clear");
    printf("X Axis position: %.4f\n", POSX);
    printf("Z Axis position: %.4f\n", POSZ);
    printf("R : Reset\n");
    printf("S : Emergency Stop\n");
    fflush(stdout);
}

int main(int argc, char *argv[])
{
    struct timeval TOut; 
	char CharsInput[SZ], MotorXValue[SZ], MotorZValue[SZ], Buff[SZ];	
	TOut.tv_usec = 100;
    fd_set ReadFileDescription;
    fd_set ReadFileDescriptionInput;
    char *XinspectFifo = "/tmp/XInspect";
    mkfifo(XinspectFifo, 0666);
	char *ZinspectFifo = "/tmp/ZInspect";
	mkfifo(ZinspectFifo, 0666);
    char *VMotorXFifo = "/tmp/VmotorX";
	mkfifo(VMotorXFifo, 0666);
    char *VMotorZFifo = "/tmp/VmotorZ";
	mkfifo(VMotorZFifo, 0666);
    char *InspectRProcessFifo = "/tmp/InspectReadProcess";
	mkfifo(InspectRProcessFifo, 0666);
    char *InspectWProcessFifo= "/tmp/InspectWriteProcess";
	mkfifo(InspectWProcessFifo, 0666);
    char *InspectMotorXFifo = "/tmp/motorXInspect";
	mkfifo(InspectMotorXFifo, 0666);
    char *InspectMotorZFifo = "/tmp/motorZInspect";
	mkfifo(InspectMotorZFifo, 0666);    
    int FileWatchdogProcessId = open(InspectRProcessFifo, O_RDONLY);
    read(FileWatchdogProcessId, Buff, SZ);
    WatchdogPointer = atoi(Buff);
    close(FileWatchdogProcessId);
    int FileDescriptionInspectProcessId = open(InspectWProcessFifo, O_WRONLY);
    sprintf(Buff, "%d", (int)getpid());
    write(FileDescriptionInspectProcessId, Buff, SZ);
    close(FileDescriptionInspectProcessId);
    FileDescX = open(XinspectFifo, O_WRONLY);
    FileDescZ = open(ZinspectFifo, O_WRONLY);
    FileDescriptionInput = fileno(stdin);
    int FileDescriptionX_i = open(InspectMotorXFifo, O_RDONLY);
    int FileDescriptionZ_i = open(InspectMotorZFifo, O_RDONLY);
    read(FileDescriptionX_i, Buff, SZ);
    MotorXProccessID = atoi(Buff);
    read(FileDescriptionZ_i, Buff, SZ);
    MotorZProccessID = atoi(Buff);
    close(FileDescriptionX_i);
    close(FileDescriptionZ_i);
    FileDescVX = open(VMotorXFifo, O_RDONLY);
    FileDescVZ = open(VMotorZFifo, O_RDONLY);
    Show();
    while (1)
    {
		FD_ZERO(&ReadFileDescription);
        FD_ZERO(&ReadFileDescriptionInput);
		FD_SET(FileDescVX, &ReadFileDescription);
        FD_SET(FileDescVZ, &ReadFileDescription);
        FD_SET(fileno(stdin), &ReadFileDescriptionInput);
        TOut.tv_sec = 0;
        TOut.tv_usec = 100000;       
		
		int check = select(FD_SETSIZE + 1, &ReadFileDescriptionInput, NULL, NULL, &TOut);		
		if(check==0)
		{
		}
		else if(check== -1)
		{
			perror("Error happend");
            fflush(stdout);
		}
		else
		{
			read(FileDescriptionInput, CharsInput, SZ);
			CharsInput[strcspn(CharsInput, "\n")] = 0;
            fflush(stdin);
			if (strlen(CharsInput) > 1)
            {
                Show();                
                fflush(stdout);
            }
			else
			{
				char cmdStr[80];
                sprintf(cmdStr, "%d", CharsInput[0]);
                switch (CharsInput[0])
                {
                case 'R': 
                case 'r': 
                    Show();
                    printf("Reset Command Sent\n");
                    fflush(stdout);
                    write(FileDescX, cmdStr, strlen(cmdStr) + 1);
                    write(FileDescZ, cmdStr, strlen(cmdStr) + 1);
                    kill(WatchdogPointer, SIGUSR1);
                    break;

                case 'S': 
                case 's': 
                    Show();
                    printf("Emergency Stop Command Sent\n");
                    fflush(stdout);
                    kill(MotorXProccessID, SIGUSR1);
                    kill(MotorZProccessID, SIGUSR1);
                    kill(WatchdogPointer, SIGUSR1);
                    break;
                default:
                    Show();
                    fflush(stdout);
                    break;
                }
			}
		}
		check = select(FD_SETSIZE + 1, &ReadFileDescription, NULL, NULL, &TOut);		
		if(check == 0)
		{
		}
		else if(check == -1)
		{
			perror("Error happend");
            fflush(stdout);
		}
		else
		{
			if (FD_ISSET(FileDescVX, &ReadFileDescription))
            {
                read(FileDescVX, MotorXValue, SZ);
                POSX = atof(MotorXValue);
            }
            if (FD_ISSET(FileDescVZ, &ReadFileDescription))
            {             
                read(FileDescVZ, MotorZValue, SZ);
                POSZ = atof(MotorZValue);
            }
            Show();
		}		
    }
    close(FileDescX);
	close(FileDescZ);
	close(FileDescVX);
	close(FileDescVZ);
    unlink(XinspectFifo);    
    unlink(ZinspectFifo);    
    unlink(VMotorXFifo);    
    unlink(VMotorZFifo);
    return 0;
}