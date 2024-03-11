#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>

#define SZ 1000
int tmp = 0;
int CommandProcess, InspectionProcess, MotorXProcess, MotorZProcess, WatchdogProcess;
char Buff[SZ];
int RunProcess(const char *p, char **MyArgs)
{
    pid_t ChildProcessId = fork();
    if (ChildProcessId != 0)
        return ChildProcessId;
    else
    {
        execvp(p, MyArgs);
        perror("Error execution!");
        return 1;
    }
}

void TerminateProcesses(int s)
{
    kill(SIGINT, MotorXProcess);
    kill(SIGINT, MotorZProcess);
    kill(SIGINT, CommandProcess);
    kill(SIGINT, InspectionProcess);
    kill(SIGINT, WatchdogProcess);
    tmp = 1;
}

int main(int argc, char *argv[])
{   
	char myConsole[] = "/usr/bin/konsole"; 
    FILE *LogFile = fopen("Log.txt", "w"); fclose(LogFile);
    signal(SIGINT, TerminateProcesses);	
    char *mxArgs[] = {"./MotorX", "", (char *)NULL};
    MotorXProcess = RunProcess("./MotorX", mxArgs);	
	char *mzArgs[] = {"./MotorZ", "", (char *)NULL};
    MotorZProcess = RunProcess("./MotorZ", mzArgs);	
	char *cmdArgs[] = {"/usr/bin/konsole", "-e", "./CmdConsole", "", (char *)NULL};
    CommandProcess = RunProcess(myConsole, cmdArgs);	
    char *inspArgs[] = {"/usr/bin/konsole", "-e", "./InspectionConsole", "", (char *)NULL};
	InspectionProcess = RunProcess(myConsole, inspArgs);	
	char *wdogArgs[] = {"./Watchdog", "", (char *)NULL};
    WatchdogProcess = RunProcess("./Watchdog", wdogArgs);
    while (tmp == 0){};
    return 0;
}
