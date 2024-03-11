#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>

#define SZ 1000
float PositionInit = 0;
float PositionMax = 1;
float MovedDistance = 0.10;
float ToMove = 1;
int FDescriptionInsp, FDescriptionCmd, FDescriptionZ , WatchdogPointer;
char Buff[SZ], LastCMD[SZ], LastInspect[SZ];
FILE *LogFile;

void SignalForResetFunc(int sig)
{
    fprintf(LogFile, "MotorZ : (command) Reset\n");
    fflush(LogFile);
    sprintf(LastInspect, "%d", 'r');
}

void SignalForEmrgStopFunc(int s)
{
    fprintf(LogFile, "MotorZ : (command) Emergency Stop\n");
    fflush(LogFile);
    sprintf(Buff, "%f", PositionInit);
    write(FDescriptionZ, Buff, strlen(Buff) + 1);
    strcpy(LastInspect, "");
    strcpy(LastCMD, "");
}

int main(int argc, char *argv[])
{
    struct timeval TOut;
    fd_set fdRead;
	float RndError, Mov;
    srand(time(NULL));
    signal(SIGUSR1, SignalForEmrgStopFunc);
    signal(SIGUSR2, SignalForResetFunc);
    char *mZ_Fifo = "/tmp/CMDMotorZ";
	mkfifo(mZ_Fifo, 0666);
    char *ZinspectFifo = "/tmp/ZInspect";
	mkfifo(ZinspectFifo, 0666);
    char *VMotorZFifo = "/tmp/VmotorZ";
	mkfifo(VMotorZFifo, 0666);
    char *WatchDogZFifo = "/tmp/WatchdogZ";
	mkfifo(WatchDogZFifo, 0666);
    char *MotorZPidFifo = "/tmp/motorZpid";
	mkfifo(MotorZPidFifo, 0666);
    char *InspectMotorZFifo = "/tmp/motorZInspect";
    mkfifo(InspectMotorZFifo, 0666);
    int WatchdogPidFil = open(WatchDogZFifo, O_RDONLY);
    read(WatchdogPidFil, Buff, SZ);
    WatchdogPointer = atoi(Buff);
    close(WatchdogPidFil);
    FDescriptionCmd = open(mZ_Fifo, O_RDONLY);
    FDescriptionInsp = open(ZinspectFifo, O_RDONLY);
    int MotorZpidi = open(InspectMotorZFifo, O_WRONLY);
    sprintf(Buff, "%d", (int)getpid());
    write(MotorZpidi, Buff, SZ);
    close(MotorZpidi);
    int MotorZpid = open(MotorZPidFifo, O_WRONLY);
    sprintf(Buff, "%d", (int)getpid());
    write(MotorZpid, Buff, SZ);
    close(MotorZpid);
    FDescriptionZ = open(VMotorZFifo, O_WRONLY);
	LogFile = fopen("Log.txt", "a");
    while (1)
    {
        TOut.tv_usec = 100000;
        FD_ZERO(&fdRead);
        FD_SET(FDescriptionCmd, &fdRead);
        FD_SET(FDescriptionInsp, &fdRead);
        RndError = (float)(-20 + rand() % 40) / 1000;
		
		int ch = select(FD_SETSIZE + 1, &fdRead, NULL, NULL, &TOut);
		if(ch==-1)
		{
			fprintf(LogFile, "Error in MotorZ\n");
            fflush(LogFile);
		}
		else if(ch==0)
		{
			switch (atoi(LastCMD))
            {
            case 'G': 
			case 'g':
                Mov = -MovedDistance + RndError;
                if (PositionInit + Mov < 0)
                {
                    PositionInit = 0;
                    sprintf(Buff, "%f", PositionInit);
                    write(FDescriptionZ, Buff, strlen(Buff) + 1);
                    strcpy(LastCMD, "");
                    sleep(ToMove);
                }
                else
                {
                    PositionInit += Mov;
                    sprintf(Buff, "%f", PositionInit);
                    write(FDescriptionZ, Buff, strlen(Buff) + 1);
                    sleep(ToMove);
                }
                fprintf(LogFile, "Z Axis = %f\n", PositionInit);
                fflush(LogFile);
                kill(WatchdogPointer, SIGUSR1);
                break;
            case 'T': 
			case 't':
                Mov = MovedDistance + RndError;
                if (PositionInit + Mov > PositionMax)
                {
                    PositionInit = PositionMax;
                    sprintf(Buff, "%f", PositionInit);
                    write(FDescriptionZ, Buff, strlen(Buff) + 1);
                    strcpy(LastCMD, "");
                    sleep(ToMove);
                }
                else
                {
                    PositionInit += Mov;
                    sprintf(Buff, "%f", PositionInit);
                    write(FDescriptionZ, Buff, strlen(Buff) + 1);
                    sleep(ToMove);
                }
                fprintf(LogFile, "Z Axis = %f\n", PositionInit);
                fflush(LogFile);
                kill(WatchdogPointer, SIGUSR1);
                break;
            case 'Z': 
			case 'z':
                write(FDescriptionZ, Buff, strlen(Buff) + 1);
                fprintf(LogFile, "Z Axis = %f\n", PositionInit);
                kill(WatchdogPointer, SIGUSR1);
                strcpy(LastCMD, "");
                sleep(ToMove);
                break;
            default:
                break;
            }
            if (atoi(LastInspect) == 'r' || atoi(LastInspect) == 'R')
            {
                Mov = -(5 * MovedDistance) + RndError;
                if (PositionInit + Mov <= 0)
                {
                    PositionInit = 0;
                    sprintf(Buff, "%f", PositionInit);
                    write(FDescriptionZ, Buff, strlen(Buff) + 1);
                    strcpy(LastInspect, "");
                    kill(WatchdogPointer, SIGUSR1);
                    sleep(ToMove);
                }
                else
                {
                    PositionInit += Mov;
                    sprintf(Buff, "%f", PositionInit);
                    write(FDescriptionZ, Buff, strlen(Buff) + 1);
                    kill(WatchdogPointer, SIGUSR1);
                    sleep(ToMove);
                }
                fprintf(LogFile, "Z Axis = %f\n", PositionInit);
                fflush(LogFile);
            }			
		}
		else
		{
			if (FD_ISSET(FDescriptionCmd, &fdRead))
                read(FDescriptionCmd, LastCMD, SZ);
            if (FD_ISSET(FDescriptionInsp, &fdRead))
            {
                read(FDescriptionInsp, LastInspect, SZ);
                strcpy(LastCMD, "");
            }
		}
    }
    fclose(LogFile);
    close(FDescriptionCmd);    
    close(FDescriptionInsp);    
	close(FDescriptionZ);
	unlink(mZ_Fifo);
	unlink(ZinspectFifo);
    unlink(VMotorZFifo);
    return 0;
}