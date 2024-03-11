#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#define SZ 80
FILE *LogFile;
time_t TimeCh;
char Buff[SZ];
int MotorXProcess, MotorZProcess, InspectionProcess, CommandProcess;

void SignalShow(int s)
{
    fprintf(LogFile, "Signal Received\n");
    fflush(LogFile);
    TimeCh = time(NULL);
}

int main(int argc, char *argv[])
{
    signal(SIGUSR1, SignalShow);
    LogFile = fopen("Log.txt", "a");

    // 1. File path creation
    char *WatchdogFifo = "/tmp/pWatchdog";
	mkfifo(WatchdogFifo, 0666);
    char *InspectWProcessFifo = "/tmp/InspectReadProcess";
	mkfifo(InspectWProcessFifo, 0666);
    char *WatchDogXFifo = "/tmp/WatchdogX";
	mkfifo(WatchDogXFifo, 0666);
    char *WatchDogZFifo = "/tmp/WatchdogZ";
	mkfifo(WatchDogZFifo, 0666);
    char *CmdFifo = "/tmp/pCommand";
	mkfifo(CmdFifo, 0666);
    char *InspectRProcessFifo = "/tmp/InspectWriteProcess";
	mkfifo(InspectRProcessFifo, 0666);
    char *MotorZPidFifo = "/tmp/motorZpid";
	mkfifo(MotorXPidFifo, 0666);
    char *MotorXPidFifo = "/tmp/motorXpid";
    mkfifo(MotorZPidFifo, 0666);
    int WatchdogCmdProcessFile = open(WatchdogFifo, O_WRONLY);
    int WatchdogInspectProcessFile = open(InspectWProcessFifo, O_WRONLY);
    int WatchdogMotorXProcessFile = open(WatchDogXFifo, O_WRONLY);
    int WatchdogMotorZProcessFile = open(WatchDogZFifo, O_WRONLY);
    sprintf(Buff, "%d", (int)getpid()); write(WatchdogCmdProcessFile, Buff, strlen(Buff) + 1);
    sprintf(Buff, "%d", (int)getpid()); write(WatchdogInspectProcessFile, Buff, strlen(Buff) + 1);
    sprintf(Buff, "%d", (int)getpid()); write(WatchdogMotorXProcessFile, Buff, strlen(Buff) + 1);
    sprintf(Buff, "%d", (int)getpid()); write(WatchdogMotorZProcessFile, Buff, strlen(Buff) + 1);
    close(WatchdogCmdProcessFile);
	close(WatchdogInspectProcessFile);
	close(WatchdogMotorXProcessFile);
	close(WatchdogMotorZProcessFile);
    unlink(WatchdogFifo);    
    unlink(InspectWProcessFifo);    
    unlink(WatchDogXFifo);    
    unlink(WatchDogZFifo);
    fprintf(LogFile, "Watchdog : Sending Process ID to other processes\n");
    fflush(LogFile);	
    int FileCmdFifo = open(CmdFifo, O_RDONLY);
    int FileInspectRProcessFifo = open(InspectRProcessFifo, O_RDONLY);
    int FileMotorXPidFifo = open(MotorXPidFifo, O_RDONLY);
    int FileMotorZPidFifo = open(MotorZPidFifo, O_RDONLY);
    read(FileCmdFifo, Buff, SZ); CommandProcess = atoi(Buff);
    read(FileInspectRProcessFifo, Buff, SZ); InspectionProcess = atoi(Buff);
    read(FileMotorXPidFifo, Buff, SZ); MotorXProcess = atoi(Buff);
    read(FileMotorZPidFifo, Buff, SZ); MotorZProcess = atoi(Buff);
    close(FileCmdFifo);
    close(FileInspectRProcessFifo);
    close(FileMotorXPidFifo);
    close(FileMotorZPidFifo);
    fprintf(LogFile, "Watchdog : Reading other processes process ID\n");
    fflush(LogFile);
    TimeCh = time(NULL);
    while (1)
    {
        sleep(10);
        if (difftime(time(NULL), TimeCh) >= 60)
        {
            kill(MotorXProcess, SIGUSR2);
            kill(MotorZProcess, SIGUSR2);
            TimeCh = time(NULL);
        }
    }    
    fclose(LogFile);
    unlink(CmdFifo);
    unlink(InspectRProcessFifo);
    unlink(MotorXPidFifo);
    unlink(MotorZPidFifo);
    return 0;
}