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
int FDescriptionInsp,FDescriptionCmd,FDescriptionX,WatchdogPointer;
float ToMove = 1;        
char Buff[SZ], LastCMD[SZ], LastInspect[SZ];
float PositionMax = 1;   
float PositionInit = 0;  
float MovedDistance = 0.10; 
FILE *LogFile;

void SignalForResetFunc(int s)
{
    fprintf(LogFile, "MotorX : (command) Reset\n");
    fflush(LogFile);
    sprintf(LastInspect, "%d", 'r');
}

void SignalForEmrgStopFunc(int s)
{
    fprintf(LogFile, "MotorX : (command) Emergency Stop\n");
    fflush(LogFile);
    sprintf(Buff, "%f", PositionInit);
    write(FDescriptionX, Buff, strlen(Buff) + 1);
    strcpy(LastCMD, "");
	strcpy(LastInspect, "");    
}

int main(int argc, char *argv[])
{    
    struct timeval TOut;
	float RndError, Mov;
    fd_set fdRead;
    srand(time(NULL));
    signal(SIGUSR1, SignalForEmrgStopFunc);
    signal(SIGUSR2, SignalForResetFunc);
    char *mX_Fifo = "/tmp/CMDMotorX";
	mkfifo(mX_Fifo, 0666);
    char *XinspectFifo = "/tmp/XInspect";
    mkfifo(XinspectFifo, 0666);
	char *VMotorXFifo = "/tmp/VmotorX";
	mkfifo(VMotorXFifo, 0666);
    char *WatchDogXFifo = "/tmp/WatchdogX";
	mkfifo(WatchDogXFifo, 0666);
    char *MotorXPidFifo  = "/tmp/motorXpid";
	mkfifo(MotorXPidFifo , 0666);
    char *InspectMotorXFifo = "/tmp/motorXInspect";
	mkfifo(InspectMotorXFifo, 0666);
    int WatchdogPidFil = open(WatchDogXFifo, O_RDONLY);
    read(WatchdogPidFil, Buff, SZ);
    WatchdogPointer = atoi(Buff);
    close(WatchdogPidFil);
    FDescriptionCmd = open(mX_Fifo, O_RDONLY);
    FDescriptionInsp = open(XinspectFifo, O_RDONLY);    
    int MotorXpidi = open(InspectMotorXFifo, O_WRONLY);
    sprintf(Buff, "%d", (int)getpid());
    write(MotorXpidi, Buff, SZ);
    close(MotorXpidi);
    int MotorXpid = open(MotorXPidFifo , O_WRONLY);
    sprintf(Buff, "%d", (int)getpid());
    write(MotorXpid, Buff, SZ);
    close(MotorXpid);
    FDescriptionX = open(VMotorXFifo, O_WRONLY);
    LogFile = fopen("Log.txt", "a");    
    while (1)
    {
        TOut.tv_usec = 100000;                
        FD_ZERO(&fdRead);
        FD_SET(FDescriptionInsp, &fdRead);
        FD_SET(FDescriptionCmd, &fdRead);
        RndError = (float)(-20 + rand() % 40) / 1000;        
        //Case -1: In case of error
        //Case 0: In case the TOut has been reached        
		
		int ch = select(FD_SETSIZE + 1, &fdRead, NULL, NULL, &TOut);
		if(ch==-1)
		{
			fprintf(LogFile, "Error in MotorX\n");
            fflush(LogFile);
		}
		else if(ch==0)
		{
			switch (atoi(last_ipcomm))
            {
			case 'F': 
			case 'f':
                Mov = -MovedDistance + RndError;
                if (PositionInit + Mov < 0)
                {
                    PositionInit = 0;
                    sprintf(Buff, "%f", PositionInit);
                    write(FDescriptionX, Buff, strlen(Buff) + 1);
                    strcpy(LastCMD, "");
                    sleep(ToMove);
                }
                else
                {
                    PositionInit += Mov;
                    sprintf(Buff, "%f", PositionInit);
                    write(FDescriptionX, Buff, strlen(Buff) + 1);
                    sleep(ToMove);
                }
                fprintf(LogFile, "X Axis = %f\n", PositionInit);
                fflush(LogFile);
                kill(WatchdogPointer, SIGUSR1);
                break;

            case 'H': 
			case 'h':
                Mov = MovedDistance + RndError;
                if (PositionInit + Mov > PositionMax)
                {
                    PositionInit = PositionMax;
                    sprintf(Buff, "%f", PositionInit);
                    write(FDescriptionX, Buff, strlen(Buff) + 1);
                    strcpy(LastCMD, "");
                    sleep(ToMove);
                }
                else
                {
                    PositionInit += Mov;
                    sprintf(Buff, "%f", PositionInit);
                    write(FDescriptionX, Buff, strlen(Buff) + 1);
                    sleep(ToMove);
                }
                fprintf(LogFile, "X Axis = %f\n", PositionInit);
                fflush(LogFile);
                kill(WatchdogPointer, SIGUSR1);
                break;
            case 'X': 
			case 'x':
                write(FDescriptionX, Buff, strlen(Buff) + 1);
                fprintf(LogFile, "X Axis = %f\n", PositionInit);
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
                    write(FDescriptionX, Buff, strlen(Buff) + 1);
                    strcpy(LastInspect, "");
                    kill(WatchdogPointer, SIGUSR1);
                    sleep(ToMove);
                }
                else
                {
                    PositionInit += Mov;
                    sprintf(Buff, "%f", PositionInit);
                    write(FDescriptionX, Buff, strlen(Buff) + 1);
                    kill(WatchdogPointer, SIGUSR1);
                    sleep(ToMove);
                }
                fprintf(LogFile, "X Axis = %f\n", PositionInit);
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
    close(FDescriptionX);
	unlink(XinspectFifo);
    unlink(mX_Fifo);
	unlink(VMotorXFifo);
    return 0;
}